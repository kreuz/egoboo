//********************************************************************************************
//*
//*    This file is part of Egoboo.
//*
//*    Egoboo is free software: you can redistribute it and/or modify it
//*    under the terms of the GNU General Public License as published by
//*    the Free Software Foundation, either version 3 of the License, or
//*    (at your option) any later version.
//*
//*    Egoboo is distributed in the hope that it will be useful, but
//*    WITHOUT ANY WARRANTY; without even the implied warranty of
//*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//*    General Public License for more details.
//*
//*    You should have received a copy of the GNU General Public License
//*    along with Egoboo.  If not, see <http://www.gnu.org/licenses/>.
//*
//********************************************************************************************

/// @file  game/graphic_prt.c
/// @brief Particle system drawing and management code.
/// @details

#include "egolib/bbox.h"
#include "game/graphic_prt.h"
#include "game/renderer_3d.h"
#include "game/game.h"
#include "game/lighting.h"
#include "game/egoboo.h"
#include "game/Graphics/CameraSystem.hpp"
#include "game/Entities/_Include.hpp"
#include "game/CharacterMatrix.h"

//--------------------------------------------------------------------------------------------


int ptex_w[2] = { 256, 256 };
int ptex_h[2] = { 256, 256 };
float ptex_wscale[2] = { 1.0f, 1.0f };
float ptex_hscale[2] = { 1.0f, 1.0f };

float CALCULATE_PRT_U0(int IDX, int CNT)  {
    return (((.05f + ((CNT)& 15)) / 16.0f)*ptex_wscale[IDX]);
}

float CALCULATE_PRT_U1(int IDX, int CNT)  {
    return (((.95f + ((CNT)& 15)) / 16.0f)*ptex_wscale[IDX]);
}

float CALCULATE_PRT_V0(int IDX, int CNT)  {
    return (((.05f + ((CNT) >> 4)) / 16.0f) * ((float)ptex_w[IDX] / (float)ptex_h[IDX])*ptex_hscale[IDX]);
}

float CALCULATE_PRT_V1(int IDX, int CNT) {
    return (((.95f + ((CNT) >> 4)) / 16.0f) * ((float)ptex_w[IDX] / (float)ptex_h[IDX])*ptex_hscale[IDX]);
}

void prt_set_texture_params(const std::shared_ptr<const Ego::Texture>& texture, uint8_t type)
{
    int index;

    switch(type) {
        case SPRITE_ALPHA:
            index = 0;
        break;
        case SPRITE_LIGHT:
            index = 1;
        break;
        default:
            throw std::invalid_argument("invalid particle type");
    }

    ptex_w[index] = texture->getSourceWidth();
    ptex_h[index] = texture->getSourceHeight();
    ptex_wscale[index] = static_cast<float>(texture->getSourceWidth()) / static_cast<float>(texture->getWidth());
    ptex_hscale[index] = static_cast<float>(texture->getSourceHeight()) / static_cast<float>(texture->getHeight());
}

//--------------------------------------------------------------------------------------------
static gfx_rv prt_instance_update(Camera& camera, const ParticleRef particle, Uint8 trans, bool do_lighting);
static void calc_billboard_verts(Ego::VertexBuffer& vb, prt_instance_t& pinst, float size, bool do_reflect);
static void draw_one_attachment_point(Ego::Graphics::ObjectGraphics& inst, int vrt_offset);
static void prt_draw_attached_point(const std::shared_ptr<Ego::Particle> &bdl_prt);
static void render_prt_bbox(const std::shared_ptr<Ego::Particle> &bdl_prt);

//--------------------------------------------------------------------------------------------

gfx_rv render_one_prt_solid(const ParticleRef iprt)
{
    /// @author BB
    /// @details Render the solid version of the particle

    const std::shared_ptr<Ego::Particle> &pprt = ParticleHandler::get()[iprt];
    if (pprt == nullptr || pprt->isTerminated())
    {
        gfx_error_add(__FILE__, __FUNCTION__, __LINE__, iprt.get(), "invalid particle");
        return gfx_error;
    }

    // if the particle is hidden, do not continue
    if (pprt->isHidden()) return gfx_fail;

    // if the particle instance data is not valid, do not continue
    if (!pprt->inst.valid) return gfx_fail;
    prt_instance_t& pinst = pprt->inst;

    // only render solid sprites
    if (SPRITE_SOLID != pprt->type) return gfx_fail;

    Ego::Renderer::get().setWorldMatrix(Matrix4f4f::identity());
    {
        Ego::OpenGL::PushAttrib pa(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT);
        {
            auto& renderer = Ego::Renderer::get();
            // Use the depth test to eliminate hidden portions of the particle
            renderer.setDepthTestEnabled(true);
            renderer.setDepthFunction(Ego::CompareFunction::Less);                                   // GL_DEPTH_BUFFER_BIT

            // enable the depth mask for the solid portion of the particles
            renderer.setDepthWriteEnabled(true);

            // draw draw front and back faces of polygons
            renderer.setCullingMode(Ego::CullingMode::None);

            // Since the textures are probably mipmapped or minified with some kind of
            // interpolation, we can never really turn blending off.
            renderer.setBlendingEnabled(true);
            renderer.setBlendFunction(Ego::BlendFunction::SourceAlpha, Ego::BlendFunction::OneMinusSourceAlpha);

            // only display the portion of the particle that is 100% solid
            renderer.setAlphaTestEnabled(true);
            renderer.setAlphaFunction(Ego::CompareFunction::Equal, 1.0f);

            renderer.getTextureUnit().setActivated(ParticleHandler::get().getTransparentParticleTexture().get());

            renderer.setColour(Ego::Math::Colour4f(pinst.fintens, pinst.fintens, pinst.fintens, 1.0f));

            // billboard for the particle
            auto vb = std::make_shared<Ego::VertexBuffer>(4, Ego::VertexFormatFactory::get<Ego::VertexFormat::P3FT2F>());
            calc_billboard_verts(*vb, pinst, pinst.size, false);

            renderer.render(*vb, Ego::PrimitiveType::TriangleFan, 0, 4);
        }
    }

    return gfx_success;
}

gfx_rv render_one_prt_trans(const ParticleRef iprt)
{
    /// @author BB
    /// @details do all kinds of transparent sprites next

    const std::shared_ptr<Ego::Particle> &pprt = ParticleHandler::get()[iprt];

    if (pprt == nullptr || pprt->isTerminated())
    {
        gfx_error_add(__FILE__, __FUNCTION__, __LINE__, iprt.get(), "invalid particle");
        return gfx_error;
    }

    // if the particle is hidden, do not continue
    if (pprt->isHidden()) return gfx_fail;

    // if the particle instance data is not valid, do not continue
    if (!pprt->inst.valid) return gfx_fail;
    prt_instance_t& inst = pprt->inst;

    {
        Ego::Renderer::get().setWorldMatrix(Matrix4f4f::identity());
        Ego::OpenGL::PushAttrib pa(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT);
        {
            auto& renderer = Ego::Renderer::get();
            // Do not write into the depth buffer.
            renderer.setDepthWriteEnabled(false);

            // Enable depth test: Incoming fragment's depth value must be less or equal.
            renderer.setDepthTestEnabled(true);
            renderer.setDepthFunction(Ego::CompareFunction::LessOrEqual);

            // Draw front-facing and back-facing polygons.
            renderer.setCullingMode(Ego::CullingMode::None);

            Ego::Math::Colour4f particleColour;

            switch(pprt->type)
            {
                // Solid sprites.
                case SPRITE_SOLID:
                {
                    // Do the alpha blended edge ("anti-aliasing") of the solid particle.
                    // Only display the alpha-edge of the particle.
                    renderer.setAlphaTestEnabled(true);
                    renderer.setAlphaFunction(Ego::CompareFunction::Less, 1.0f);

                    renderer.setBlendingEnabled(true);
                    renderer.setBlendFunction(Ego::BlendFunction::SourceAlpha, Ego::BlendFunction::OneMinusSourceAlpha);

                    particleColour = Ego::Math::Colour4f(inst.fintens, inst.fintens, inst.fintens, 1.0f);

                    renderer.getTextureUnit().setActivated(ParticleHandler::get().getTransparentParticleTexture().get());
                }
                break;

                // Light sprites.
                case SPRITE_LIGHT:
                {
                    //Is particle invisible?
                    if(inst.fintens * inst.falpha <= 0.0f) {
                        return gfx_success;
                    }

                    renderer.setAlphaTestEnabled(false);
                    renderer.setBlendingEnabled(true);
                    renderer.setBlendFunction(Ego::BlendFunction::One, Ego::BlendFunction::One);

                    particleColour = Ego::Math::Colour4f(1.0f, 1.0f, 1.0f, inst.fintens * inst.falpha);

                    renderer.getTextureUnit().setActivated(ParticleHandler::get().getLightParticleTexture().get());
                }
                break;

                // Transparent sprites.
                case SPRITE_ALPHA:
                {
                    //Is particle invisible?
                    if(inst.falpha <= 0.0f) {
                        return gfx_success;
                    }

                    // do not display the completely transparent portion
                    renderer.setAlphaTestEnabled(true);
                    renderer.setAlphaFunction(Ego::CompareFunction::Greater, 0.0f);

                    renderer.setBlendingEnabled(true);
                    renderer.setBlendFunction(Ego::BlendFunction::SourceAlpha, Ego::BlendFunction::OneMinusSourceAlpha);

                    particleColour = Ego::Math::Colour4f(inst.fintens, inst.fintens, inst.fintens, inst.falpha);

                    renderer.getTextureUnit().setActivated(ParticleHandler::get().getTransparentParticleTexture().get());
                }
                break;

                // unknown type
                default:
                    return gfx_error;
                break;
            }

            auto vb = std::make_shared<Ego::VertexBuffer>(4, Ego::VertexFormatFactory::get<Ego::VertexFormat::P3FT2F>());
            calc_billboard_verts(*vb, inst, inst.size, false);

            renderer.setColour(particleColour);

            // Go on and draw it
            renderer.render(*vb, Ego::PrimitiveType::TriangleFan, 0, 4);
        }
    }

    return gfx_success;
}

gfx_rv render_one_prt_ref(const ParticleRef iprt)
{
    /// @author BB
    /// @details render one particle
    const std::shared_ptr<Ego::Particle>& pprt = ParticleHandler::get()[iprt];
    if(!pprt || pprt->isTerminated()) {
        gfx_error_add(__FILE__, __FUNCTION__, __LINE__, iprt.get(), "invalid particle");
        return gfx_error;
    }

    // if the particle is hidden, do not continue
    if (pprt->isHidden()) return gfx_fail;

    if (!pprt->inst.valid || !pprt->inst.ref_valid) return gfx_fail;
    prt_instance_t& inst = pprt->inst;

    //Calculate the fadeoff factor depending on how high above the floor the particle is 
    float fadeoff = 255.0f - (pprt->enviro.floor_level - inst.ref_pos.z()); //255 - distance over ground
    fadeoff *= 0.5f;
    fadeoff = Ego::Math::constrain(fadeoff*INV_FF<float>(), 0.0f, 1.0f);

    if (fadeoff > 0.0f)
    {
        Ego::Renderer::get().setWorldMatrix(Matrix4f4f::identity());
        {
            Ego::OpenGL::PushAttrib pa(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT | GL_CURRENT_BIT);
            {
                Ego::Colour4f particle_colour;

                auto& renderer = Ego::Renderer::get();
                // don't write into the depth buffer (disable glDepthMask for transparent objects)
                renderer.setDepthWriteEnabled(false); // ENABLE_BIT

                // do not draw hidden surfaces
                renderer.setDepthTestEnabled(true);
                renderer.setDepthFunction(Ego::CompareFunction::LessOrEqual);

                // draw draw front and back faces of polygons
                renderer.setCullingMode(Ego::CullingMode::None);

                // do not display the completely transparent portion
                renderer.setAlphaTestEnabled(true);
                renderer.setAlphaFunction(Ego::CompareFunction::Greater, 0.0f);

                renderer.setBlendingEnabled(true);
                renderer.setBlendFunction(Ego::BlendFunction::SourceAlpha, Ego::BlendFunction::OneMinusSourceAlpha);

                switch(pprt->type) 
                {
                    case SPRITE_LIGHT:
                    {
                        // do the light sprites
                        float alpha = fadeoff * inst.falpha;

                        //Nothing to draw?
                        if(alpha <= 0.0f) {
                            return gfx_fail;
                        }

                        particle_colour = Ego::Math::Colour4f(1.0f, 1.0f, 1.0f, alpha);

                        renderer.getTextureUnit().setActivated(ParticleHandler::get().getLightParticleTexture().get());
                    }
                    break;

                    case SPRITE_SOLID:
                    case SPRITE_ALPHA:
                    {
                        float alpha = fadeoff;
                        if (SPRITE_ALPHA == pprt->type) {
                            alpha *= inst.falpha;

                            //Nothing to draw?
                            if(alpha <= 0.0f) {
                                return gfx_fail;
                            }
                        }

                        particle_colour = Ego::Math::Colour4f(inst.fintens, inst.fintens, inst.fintens, alpha);

                        renderer.getTextureUnit().setActivated(ParticleHandler::get().getTransparentParticleTexture().get());
                    }
                    break;

                    // unknown type
                    default:
                        return gfx_fail;
                    break;
                }

                // Calculate the position of the four corners of the billboard
                // used to display the particle.
                auto vb = std::make_shared<Ego::VertexBuffer>(4, Ego::VertexFormatFactory::get<Ego::VertexFormat::P3FT2F>());
                calc_billboard_verts(*vb, inst, inst.size, true);

                renderer.setColour(particle_colour); // GL_CURRENT_BIT

                renderer.render(*vb, Ego::PrimitiveType::TriangleFan, 0, 4);
            }
        }
    }

    return gfx_success;
}

void calc_billboard_verts(Ego::VertexBuffer& vb, prt_instance_t& inst, float size, bool do_reflect)
{
    // Calculate the position and texture coordinates of the four corners of the billboard used to display the particle.

    if (vb.getNumberOfVertices() < 4)
    {
        throw std::runtime_error("vertex buffer too small");
    }
    /// @todo Add a compare by equality for vertex format descriptors and assert the vertex buffer has the required format.
    struct Vertex
    {
        float x, y, z;
        float s, t;
    };

    int i, index;
	Vector3f prt_pos, prt_up, prt_right;

    switch (inst.type)
    {
        default:
        case SPRITE_ALPHA:
            index = 0;
            break;

        case SPRITE_LIGHT:
            index = 1;
            break;
    }

    // use the pre-computed reflection parameters
    if (do_reflect)
    {
        prt_pos = inst.ref_pos;
        prt_up = inst.ref_up;
        prt_right = inst.ref_right;
    }
    else
    {
        prt_pos = inst.pos;
        prt_up = inst.up;
        prt_right = inst.right;
    }

    Vertex *v = static_cast<Vertex *>(vb.lock());

    for (i = 0; i < 4; i++)
    {
        v[i].x = prt_pos[kX];
        v[i].y = prt_pos[kY];
        v[i].z = prt_pos[kZ];
    }

    v[0].x += (-prt_right[kX] - prt_up[kX]) * size;
    v[0].y += (-prt_right[kY] - prt_up[kY]) * size;
    v[0].z += (-prt_right[kZ] - prt_up[kZ]) * size;

    v[1].x += (prt_right[kX] - prt_up[kX]) * size;
    v[1].y += (prt_right[kY] - prt_up[kY]) * size;
    v[1].z += (prt_right[kZ] - prt_up[kZ]) * size;

    v[2].x += (prt_right[kX] + prt_up[kX]) * size;
    v[2].y += (prt_right[kY] + prt_up[kY]) * size;
    v[2].z += (prt_right[kZ] + prt_up[kZ]) * size;

    v[3].x += (-prt_right[kX] + prt_up[kX]) * size;
    v[3].y += (-prt_right[kY] + prt_up[kY]) * size;
    v[3].z += (-prt_right[kZ] + prt_up[kZ]) * size;

    v[0].s = CALCULATE_PRT_U1(index, inst.image_ref);
    v[0].t = CALCULATE_PRT_V1(index, inst.image_ref);

    v[1].s = CALCULATE_PRT_U0(index, inst.image_ref);
    v[1].t = CALCULATE_PRT_V1(index, inst.image_ref);

    v[2].s = CALCULATE_PRT_U0(index, inst.image_ref);
    v[2].t = CALCULATE_PRT_V0(index, inst.image_ref);

    v[3].s = CALCULATE_PRT_U1(index, inst.image_ref);
    v[3].t = CALCULATE_PRT_V0(index, inst.image_ref);

    vb.unlock();
}

void render_all_prt_attachment()
{
    Ego::Renderer::get().setBlendingEnabled(false);

    for(const std::shared_ptr<Ego::Particle> &particle : ParticleHandler::get().iterator())
    {
        if(particle->isTerminated()) continue;
        prt_draw_attached_point(particle);
    }
}

void render_all_prt_bbox()
{
    for(const std::shared_ptr<Ego::Particle> &particle : ParticleHandler::get().iterator())
    {
        if(particle->isTerminated()) continue;
        render_prt_bbox(particle);
    }
}

void draw_one_attachment_point(Ego::Graphics::ObjectGraphics& inst, int vrt_offset)
{
    /// @author BB
    /// @details a function that will draw some of the vertices of the given character.
    ///     The original idea was to use this to debug the grip for attached items.
    uint32_t vrt = (int)inst.getVertexCount() - (int)vrt_offset;

    if (vrt >= inst.getVertexCount()) return;

    // disable the texturing so all the points will be white,
    // not the texture color of the last vertex we drawn
    Ego::Renderer::get().getTextureUnit().setActivated(nullptr);
    Ego::Renderer::get().setPointSize(5);
    Ego::Renderer::get().setViewMatrix(Matrix4f4f::identity());
    Ego::Renderer::get().setWorldMatrix(inst.getMatrix());
    GL_DEBUG(glBegin(GL_POINTS));
    {
        GL_DEBUG(glVertex3fv)(inst.getVertex(vrt).pos);
    }
    GL_DEBUG_END();
}

void prt_draw_attached_point(const std::shared_ptr<Ego::Particle>& particle)
{
    if (!particle->isAttached()) {
        return;
    }

    draw_one_attachment_point(particle->getAttachedObject()->inst, particle->attachedto_vrt_off);
}

gfx_rv update_all_prt_instance(Camera& camera)
{
    // only one update per frame
    static uint32_t instance_update = std::numeric_limits<uint32_t>::max();
    if (instance_update == update_wld) return gfx_success;
    instance_update = update_wld;

    // assume the best
    gfx_rv retval = gfx_success;

    for(const std::shared_ptr<Ego::Particle> &particle : ParticleHandler::get().iterator())
    {
        if(particle->isTerminated()) continue;
        
        prt_instance_t *pinst = &(particle->inst);

        // only do frame counting for particles that are fully activated!
        particle->frame_count++;

        if (!particle->inst.indolist)
        {
            pinst->valid = false;
            pinst->ref_valid = false;
        }
        else
        {
            // calculate the "billboard" for this particle
            if (gfx_error == prt_instance_update(camera, particle->getParticleID(), 255, true))
            {
                retval = gfx_error;
            }
        }
    }
 
    return retval;
}

gfx_rv prt_instance_t::update_vertices(prt_instance_t& inst, Camera& camera, Ego::Particle *pprt)
{
    inst.valid = false;
    inst.ref_valid = false;

    if (pprt->isTerminated())
    {
        gfx_error_add(__FILE__, __FUNCTION__, __LINE__, pprt->getParticleID().get(), "invalid particle");
        return gfx_error;
    }

    const std::shared_ptr<ParticleProfile> &ppip = pprt->getProfile();

    inst.type = pprt->type;

    inst.image_ref = (pprt->_image._start / EGO_ANIMATION_MULTIPLIER + pprt->_image._offset / EGO_ANIMATION_MULTIPLIER);


    // Set the position.
    inst.pos = pprt->getPosition();
    inst.orientation = ppip->orientation;

    // Calculate the billboard vectors for the reflections.
    inst.ref_pos = pprt->getPosition();
    inst.ref_pos[kZ] = 2 * pprt->enviro.floor_level - inst.pos[kZ];

    // get the vector from the camera to the particle
	Vector3f vfwd = inst.pos - camera.getPosition();
    vfwd.normalize();

	Vector3f vfwd_ref = inst.ref_pos - camera.getPosition();
    vfwd_ref.normalize();

    // Set the up and right vectors.
	Vector3f vup = Vector3f(0.0f, 0.0f, 1.0f), vright;
	Vector3f vup_ref = Vector3f(0.0f, 0.0f, 1.0f), vright_ref;
    if (ppip->rotatetoface && !pprt->isAttached() && (pprt->vel.length_abs() > 0))
    {
        // The particle points along its direction of travel.

        vup = pprt->vel;
        vup.normalize();

        // Get the correct "right" vector.
        vright = vfwd.cross(vup);
        vright.normalize();

        vup_ref = vup;
        vright_ref = vfwd_ref.cross(vup);
        vright_ref.normalize();
    }
    else if (prt_ori_t::ORIENTATION_B == inst.orientation)
    {
        // Use the camera up vector.
        vup = camera.getUp();
        vup.normalize();

        // Get the correct "right" vector.
        vright = vfwd.cross(vup);
        vright.normalize();

        vup_ref = vup;
        vright_ref = vfwd_ref.cross(vup);
        vright_ref.normalize();
    }
    else if (prt_ori_t::ORIENTATION_V == inst.orientation)
    {
        // Using just the global up vector here is too harsh.
        // Smoothly interpolate the global up vector with the camera up vector
        // so that when the camera is looking straight down, the billboard's plane
        // is turned by 45 degrees to the camera (instead of 90 degrees which is invisible)

        // Use the camera up vector.
		Vector3f vup_cam = camera.getUp();

        // Use the global up vector.
        vup = Vector3f(0.0f, 0.0f, 1.0f);

        // Adjust the vector so that the particle doesn't disappear if
        // you are viewing it from from the top or the bottom.
        float weight = 1.0f - std::abs(vup_cam[kZ]);
        if (vup_cam[kZ] < 0) weight *= -1;

        vup += vup_cam * weight;
        vup.normalize();

        // Get the correct "right" vector.
        vright = vfwd.cross(vup);
        vright.normalize();

        vright_ref = vfwd.cross(vup_ref);
        vright_ref.normalize();

        vup_ref = vup;
        vright_ref = vfwd_ref.cross(vup);
        vright_ref.normalize();
    }
    else if (prt_ori_t::ORIENTATION_H == inst.orientation)
    {
		Vector3f vert = Vector3f(0.0f, 0.0f, 1.0f);

        // Force right to be horizontal.
        vright = vfwd.cross(vert);

        // Force "up" to be close to the camera forward, but horizontal.
        vup = vert.cross(vright);
        //vup_ref = vert.cross(vright_ref); //TODO: JJ> is this needed?

        // Normalize them.
        vright.normalize();
        vup.normalize();

        vright_ref = vright;
        vup_ref = vup;
    }
    else if (pprt->isAttached())
    {
        Ego::Graphics::ObjectGraphics *cinst = &(pprt->getAttachedObject()->inst);

        if (chr_matrix_valid(pprt->getAttachedObject().get()))
        {
            // Use the character matrix to orient the particle.
            // Assume that the particle "up" is in the z-direction in the object's
            // body fixed axes. Should work for the gonnes & such.

            switch (inst.orientation)
            {
                case prt_ori_t::ORIENTATION_X: vup = mat_getChrForward(cinst->getMatrix()); break;
                case prt_ori_t::ORIENTATION_Y: vup = mat_getChrRight(cinst->getMatrix());   break;
                default:
                case prt_ori_t::ORIENTATION_Z: vup = mat_getChrUp(cinst->getMatrix());      break;
            }

            vup.normalize();
        }
        else
        {
            // Use the camera directions?
            switch (inst.orientation)
            {
                case prt_ori_t::ORIENTATION_X: vup = camera.getForward(); break;
                case prt_ori_t::ORIENTATION_Y: vup = camera.getRight(); break;

                default:
                case prt_ori_t::ORIENTATION_Z: vup = camera.getUp(); break;
            }
        }

        vup.normalize();

        // Get the correct "right" vector.
        vright = vfwd.cross(vup);
        vright.normalize();

        vup_ref = vup;
        vright_ref = vfwd_ref.cross(vup);
        vright_ref.normalize();
    }
    else
    {
        // Use the camera up vector.
        vup = camera.getUp();
        vup.normalize();

        // Get the correct "right" vector.
        vright = vfwd.cross(vup);
        vright.normalize();

        vup_ref = vup;
        vright_ref = vfwd_ref.cross(vup);
        vright_ref.normalize();
    }

    // Calculate the actual vectors using the particle rotation.
    /// @todo An optimization for the special case where the angle
    /// a is 0, taking advantage of the obvious fact that cos(a)=1,
    /// sin(a)=0 for a = 0. However, it is a quite special optimization,
    /// as it does not take into account the cases a = n * 360, n =
    /// ..., -1, 0, +1, ...
    if (Facing(0) == pprt->rotate)
    {
        inst.up = vup; // vup * 1 - vright * 0
        inst.right = vright; // vup * 0 + vright * 1

        inst.ref_up = vup_ref; // vup_ref * 1 - vright_ref * 0
        inst.ref_right = vright_ref; // vup_ref * 0 + vright_ref * 1
    }
    else
    {
        float cosval = std::cos(pprt->rotate);
        float sinval = std::sin(pprt->rotate);

        inst.up = vup * cosval - vright * sinval;

        inst.right = vup * sinval + vright * cosval;

        inst.ref_up = vup_ref * cosval - vright_ref * sinval;

        inst.ref_right = vup_ref * sinval + vright_ref * cosval;
    }

    // Calculate the billboard normal.
    inst.nrm = inst.right.cross(inst.up);

    // Flip the normal so that the front front of the quad is toward the camera.
    if (vfwd.dot(inst.nrm) < 0)
    {
        inst.nrm *= -1;
    }

    // Now we have to calculate the mirror-like reflection of the particles.
    // This was a bit hard to figure. What happens is that the components of the
    // up and right vectors that are in the plane of the quad and closest to the world up are reversed.
    //
    // This is easy to think about in a couple of examples:
    // 1) If the quad is like a picture frame then whatever component (up or right)
    //    that actually points in the wodld up direction is reversed.
    //    This corresponds to the case where zdot == +/- 1 in the code below.
    //
    // 2) If the particle is like a rug, then basically nothing happens since
    //    neither the up or right vectors point in the world up direction.
    //    This corresponds to 0 == ndot in the code below.
    //
    // This process does not affect the normal the length of the vector, or the
    // direction of the normal to the quad.

    {
        // The normal sense of "up".
		Vector3f world_up = Vector3f(0.0f, 0.0f, 1.0f);

        // The dot product between the normal vector and the world up vector:
        // The following statement could be optimized
        // since we know the only non-zero component of the world up vector is z.
        float ndot = inst.nrm.dot(world_up);

        // Do nothing if the quad is basically horizontal.
        if (ndot < 1.0f - 1e-6)
        {
            // Do the right vector first.
            {
                // The dot product between the right vector and the world up:
                // The following statement could be optimized
                // since we know the only non-zero component of the world up vector is z.
                float zdot = inst.ref_right.dot(world_up);

                if (std::abs(zdot) > 1e-6)
                {
                    float factor = zdot / (1.0f - ndot * ndot);
                    inst.ref_right += ((inst.nrm * ndot) - world_up) * 2.0f * factor;
                }
            }

            // Do the up vector second.
            {
                // The dot product between the up vector and the world up:
                // The following statement could be optimized
                // since we know the only non-zero component of the world up vector is z.
                float zdot = inst.ref_up.dot(world_up);

                if (std::abs(zdot) > 1e-6)
                {
                    float factor = zdot / (1.0f - ndot * ndot);
                    inst.ref_up += (inst.nrm * ndot - world_up) * 2.0f * factor;
                }
            }
        }
    }

    // Set some particle dependent properties.
    inst.scale = pprt->getScale();
    inst.size = FP8_TO_FLOAT(pprt->size) * inst.scale;

    // This instance is now completely valid.
    inst.valid = true;
    inst.ref_valid = true;

    return gfx_success;
}

Matrix4f4f prt_instance_t::make_matrix(prt_instance_t& pinst)
{
	Matrix4f4f mat = Matrix4f4f::identity();

    mat(1, 0) = -pinst.up[kX];
    mat(1, 1) = -pinst.up[kY];
    mat(1, 2) = -pinst.up[kZ];

    mat(0, 0) = pinst.right[kX];
    mat(0, 1) = pinst.right[kY];
    mat(0, 2) = pinst.right[kZ];

    mat(2, 0) = pinst.nrm[kX];
    mat(2, 1) = pinst.nrm[kY];
    mat(2, 2) = pinst.nrm[kZ];

    return mat;
}

gfx_rv prt_instance_t::update_lighting(prt_instance_t& pinst, Ego::Particle *pprt, Uint8 trans, bool do_lighting)
{
    if (!pprt)
    {
        gfx_error_add(__FILE__, __FUNCTION__, __LINE__, 0, "NULL particle");
        return gfx_error;
    }

    // To make life easier
    Uint32 alpha = trans;

    // interpolate the lighting for the origin of the object
	auto mesh = _currentModule->getMeshPointer();
	if (!mesh) {
		throw Id::RuntimeErrorException(__FILE__, __LINE__, "nullptr == mesh");
	}
    lighting_cache_t global_light;
	GridIllumination::grid_lighting_interpolate(*mesh, global_light, Vector2f(pinst.pos[kX], pinst.pos[kY]));

    // rotate the lighting data to body_centered coordinates
	Matrix4f4f mat = prt_instance_t::make_matrix(pinst);
    lighting_cache_t loc_light;
	lighting_cache_t::lighting_project_cache(loc_light, global_light, mat);

    // determine the normal dependent amount of light
    float amb, dir;
	lighting_cache_t::lighting_evaluate_cache(loc_light, pinst.nrm, pinst.pos[kZ], _currentModule->getMeshPointer()->_tmem._bbox, &amb, &dir);

    // LIGHT-blended sprites automatically glow. ALPHA-blended and SOLID
    // sprites need to convert the light channel into additional alpha
    // lighting to make them "glow"
    Sint16 self_light = 0;
    if (SPRITE_LIGHT != pinst.type)
    {
        self_light = (255 == pinst.light) ? 0 : pinst.light;
    }

    // determine the ambient lighting
    pinst.famb = 0.9f * pinst.famb + 0.1f * (self_light + amb);
    pinst.fdir = 0.9f * pinst.fdir + 0.1f * dir;

    // determine the overall lighting
    pinst.fintens = pinst.fdir * INV_FF<float>();
    if (do_lighting)
    {
        pinst.fintens += pinst.famb * INV_FF<float>();
    }
    pinst.fintens = Ego::Math::constrain(pinst.fintens, 0.0f, 1.0f);

    // determine the alpha component
    pinst.falpha = (alpha * INV_FF<float>()) * (pinst.alpha * INV_FF<float>());
    pinst.falpha = Ego::Math::constrain(pinst.falpha, 0.0f, 1.0f);

    return gfx_success;
}

gfx_rv prt_instance_update(Camera& camera, const ParticleRef particle, Uint8 trans, bool do_lighting)
{
    const std::shared_ptr<Ego::Particle> &pprt = ParticleHandler::get()[particle];
    if(!pprt) {
        gfx_error_add(__FILE__, __FUNCTION__, __LINE__, particle.get(), "invalid particle");
        return gfx_error;
    }

    prt_instance_t& pinst = pprt->inst;

    // assume the best
    gfx_rv retval = gfx_success;

    // make sure that the vertices are interpolated
    if (gfx_error == prt_instance_t::update_vertices(pinst, camera, pprt.get()))
    {
        retval = gfx_error;
    }

    // do the lighting
    if (gfx_error == prt_instance_t::update_lighting(pinst, pprt.get(), trans, do_lighting))
    {
        retval = gfx_error;
    }

    return retval;
}

void render_prt_bbox(const std::shared_ptr<Ego::Particle>& particle)
{    
    // only draw bullets
    //if ( 50 != loc_ppip->vel_hrz_pair.base ) return;

    // draw the object bounding box as a part of the graphics debug mode F7
    if ((egoboo_config_t::get().debug_developerMode_enable.getValue() && Ego::Input::InputSystem::get().isKeyDown(SDLK_F7)))
    {
        // copy the bounding volume
        oct_bb_t tmp_bb = particle->prt_max_cv;

        // determine the expanded collision volumes for both objects
        oct_bb_t exp_bb;
        phys_expand_oct_bb(tmp_bb, particle->vel, 0, 1, exp_bb);

        // shift the source bounding boxes to be centered on the given positions
        oct_bb_t loc_bb;
        loc_bb = oct_bb_t::translate(exp_bb, particle->getPosition());

        Ego::Renderer::get().getTextureUnit().setActivated(nullptr);
        Ego::Renderer::get().setColour(Ego::Math::Colour4f::white());
        Renderer3D::renderOctBB(loc_bb, true, true, Ego::Math::Colour4f::red(), Ego::Math::Colour4f::yellow());
    }
}
