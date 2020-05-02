R"(
#version 330 core 

layout (location = 0) in vec4 aColor;
layout (location = 1) in vec4 aTextureRect;
layout (location = 2) in vec2 aPosition;
layout (location = 3) in vec2 aSize;
layout (location = 4) in vec2 aRotationOrigin;
layout (location = 5) in float aRotation;
layout (location = 6) in float aTextureSlotRef;

out DATA
{
    vec4 color;
    vec2 texCoords;
	vec2 texSize;
	vec2 fragPos;
	vec2 quadrantSize;
    flat int textureSlotRef;
} vs_out;

layout (std140) uniform SharedData
{
    mat4 gameWorldVPM;
	mat4 guiVPM;
};

uniform float z;
uniform bool isGameWorldProjection;
uniform sampler2D textures[32];

void main()
{
    vs_out.color = aColor;
    vs_out.textureSlotRef = int(aTextureSlotRef);

    vec2 modelVertexPos;
    
    switch(gl_VertexID)
    {
        case 0: {
            modelVertexPos = vec2(0, 0);
            vs_out.texCoords = vec2(aTextureRect.x, aTextureRect.y + aTextureRect.w);
        } break;
        
		case 1: {
            modelVertexPos = vec2(aSize.x, 0);
            vs_out.texCoords = vec2(aTextureRect.x + aTextureRect.z, aTextureRect.y + aTextureRect.w);
        } break;

        case 2: {
            modelVertexPos = vec2(0, aSize.y);
            vs_out.texCoords = vec2(aTextureRect.x, aTextureRect.y);
        } break;

        case 3: {
            modelVertexPos = vec2(aSize.x, aSize.y);
            vs_out.texCoords = vec2(aTextureRect.x + aTextureRect.z, aTextureRect.y);
        } break;
    }

	vs_out.texSize = vec2(textureSize(textures[int(aTextureSlotRef)], 0));
	vs_out.texCoords *= vs_out.texSize;
	vs_out.fragPos = modelVertexPos;
	vs_out.quadrantSize = aSize / 2;
	
	mat4 VPM = isGameWorldProjection ? gameWorldVPM : guiVPM;
    
	gl_Position = VPM * vec4(modelVertexPos + aPosition, z, 1);
}

)"
