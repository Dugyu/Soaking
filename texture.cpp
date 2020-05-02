#include <vector>

#include "headers/common.h"
#include "headers/glsupport.h"
#include "headers/ppm.h"
#include "headers/texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "headers/stb_image.h"


using namespace std;

ImageTexture::ImageTexture(const char *ppmFileName, bool srgb) {
    int width, height;
    vector<PackedPixel> pixData;
    ppmRead(ppmFileName, width, height, pixData);

    glBindTexture(GL_TEXTURE_2D, tex);
    if (g_Gl2Compatible)
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

    glTexImage2D(GL_TEXTURE_2D, 0,
                 (!srgb) || g_Gl2Compatible ? GL_RGB : GL_SRGB, width, height,
                 0, GL_RGB, GL_UNSIGNED_BYTE, &pixData[0]);

    if (!g_Gl2Compatible)
        glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    checkGlErrors();
}

GeneralTexture::GeneralTexture(const char *imgFileName, bool srgb){
    
    int width, height, nrChannels;
    unsigned char *pixData = stbi_load(imgFileName, &width, &height, &nrChannels, 0);

    if (pixData)
    {
       glBindTexture(GL_TEXTURE_2D, tex);
       if (g_Gl2Compatible)
           glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

       glTexImage2D(GL_TEXTURE_2D, 0,
                    (!srgb) || g_Gl2Compatible ? GL_RGBA : GL_SRGB_ALPHA, width, height,
                    0, GL_RGBA, GL_UNSIGNED_BYTE, pixData);

       if (!g_Gl2Compatible)
           glGenerateMipmap(GL_TEXTURE_2D);

       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                       GL_LINEAR_MIPMAP_LINEAR);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }else{
        std::cout << "Failed to load texture" << std::endl;
    }
        
    

    checkGlErrors();
}


