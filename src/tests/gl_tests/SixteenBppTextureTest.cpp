//
// Copyright 2015 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// SixteenBppTextureTest:
//   Basic tests using 16bpp texture formats (e.g. GL_RGB565).

#include "end2end_tests/ANGLETest.h"

using namespace angle;

namespace
{

class SixteenBppTextureTest : public ANGLETest
{
  protected:
    SixteenBppTextureTest()
    {
        setWindowWidth(128);
        setWindowHeight(128);
        setConfigRedBits(8);
        setConfigGreenBits(8);
        setConfigBlueBits(8);
        setConfigAlphaBits(8);
    }

    virtual void SetUp()
    {
        ANGLETest::SetUp();

        const std::string vertexShaderSource = SHADER_SOURCE
        (
            precision highp float;
            attribute vec4 position;
            varying vec2 texcoord;

            void main()
            {
                gl_Position = vec4(position.xy, 0.0, 1.0);
                texcoord = (position.xy * 0.5) + 0.5;
            }
        );

        const std::string fragmentShaderSource2D = SHADER_SOURCE
        (
            precision highp float;
            uniform sampler2D tex;
            varying vec2 texcoord;

            void main()
            {
                gl_FragColor = texture2D(tex, texcoord);
            }
        );

        m2DProgram = CompileProgram(vertexShaderSource, fragmentShaderSource2D);
        mTexture2DUniformLocation = glGetUniformLocation(m2DProgram, "tex");
    }

    virtual void TearDown()
    {
        glDeleteProgram(m2DProgram);

        ANGLETest::TearDown();
    }

    GLuint m2DProgram;
    GLint mTexture2DUniformLocation;
};

// Simple validation test for GLRGB565 textures.
// Samples from the texture, renders to it, generates mipmaps etc.
TEST_P(SixteenBppTextureTest, RGB565Validation)
{
    GLushort pixels[4] =
    {
        0xF800, // Red
        0x07E0, // Green
        0x001F, // Blue
        0xFFE0  // Red + Green
    };

    glClearColor(0, 0, 0, 0);

    // Create a simple RGB565 texture
    GLuint tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    EXPECT_GL_NO_ERROR();

    // Supply the data to it
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 2, 2, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, pixels);
    EXPECT_GL_NO_ERROR();

    // Draw a quad using the texture
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(m2DProgram);
    glUniform1i(mTexture2DUniformLocation, 0);
    drawQuad(m2DProgram, "position", 0.5f);
    EXPECT_GL_NO_ERROR();

    // Check that it drew as expected
    EXPECT_PIXEL_EQ(0,                     0,                    255,   0,   0, 255);
    EXPECT_PIXEL_EQ(getWindowHeight() - 1, 0,                      0, 255,   0, 255);
    EXPECT_PIXEL_EQ(0,                     getWindowWidth() - 1,   0,   0, 255, 255);
    EXPECT_PIXEL_EQ(getWindowHeight() - 1, getWindowWidth() - 1, 255, 255,   0, 255);
    swapBuffers();

    // Generate mipmaps
    glGenerateMipmap(GL_TEXTURE_2D);

    // Draw a quad using the texture
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(m2DProgram);
    glUniform1i(mTexture2DUniformLocation, 0);
    drawQuad(m2DProgram, "position", 0.5f);
    EXPECT_GL_NO_ERROR();

    // Check that it drew as expected
    EXPECT_PIXEL_EQ(0,                     0,                    255,   0,   0, 255);
    EXPECT_PIXEL_EQ(getWindowHeight() - 1, 0,                      0, 255,   0, 255);
    EXPECT_PIXEL_EQ(0,                     getWindowWidth() - 1,   0,   0, 255, 255);
    EXPECT_PIXEL_EQ(getWindowHeight() - 1, getWindowWidth() - 1, 255, 255,   0, 255);
    swapBuffers();

    // Bind the texture as a framebuffer, render to it, then check the results
    GLuint fbo = 0;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    EXPECT_PIXEL_EQ(0, 0, 255, 0, 0, 255);
    EXPECT_PIXEL_EQ(1, 0, 255, 0, 0, 255);
    EXPECT_PIXEL_EQ(1, 1, 255, 0, 0, 255);
    EXPECT_PIXEL_EQ(0, 1, 255, 0, 0, 255);

    glDeleteFramebuffers(1, &fbo);
    glDeleteTextures(1, &tex);
}


// Use this to select which configurations (e.g. which renderer, which GLES major version) these tests should be run against.
ANGLE_INSTANTIATE_TEST(SixteenBppTextureTest, ES2_D3D9(),  ES2_D3D11(), ES2_D3D11_FL9_3(), ES2_OPENGL());

} // namespace