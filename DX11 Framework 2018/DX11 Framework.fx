//--------------------------------------------------------------------------------------
// File: DX11 Framework.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
//#include "LightStructs.h"
#include "LightShading.fx"

//-------------------------------------------------------------------------------------
// Texturing
//-------------------------------------------------------------------------------------

Texture2D txDiffuse : register( t0 );
Texture2D txSpecular : register (t1);
Texture2D txNightDiffuse : register (t2);

SamplerState samLinear : register(s0);


//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register( b0 )
{
	matrix World;
	matrix View;
	matrix Projection;

    //A  color vector that describes the diffuse material (RGBA)
    float4 DiffuseMtrl;

    //A colour vector that describes the diffuse light colour (RGBA)
    float4 DiffuseLight;

    //The light vector; for a directional light, this is a vector that points in the direction of the light source
    //I.e, in the opposite direction of the incoming light rays (XYZ)
    float3 LightVecW;

    float dataBreak;

    //Ambient Lighting 
    float4 AmbientLight;
    float4 AmbientMtrl;

    //Specular Lighting
    float4 SpecularMtrl;
    float4 SpecularLight;
    float SpecularPower;
    float3 EyePosW;

    Material material;
    PointLight pointLight;
    SpotLight spotLight;

}



//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    //float4 Color : COLOR0;
    float3 Norm: NORMAL;
    //float3 Posw : POSITION;
    float3 PosW : POSITION;
    float2 Tex : TEXCOORD0;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

VS_OUTPUT VS(float4 Pos : POSITION, float3 NormalL : NORMAL, float2 Tex : TEXCOORD0)
{
    
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Norm = normalize(NormalL);

    output.Pos = mul( Pos, World );
    output.PosW = normalize(EyePosW - output.Pos.xyz);

    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );
    output.Tex = Tex;

    return output;

}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_Target
{
    // Convert from local space to world space 
   // W component of vector is 0 as vectors cannot be translated


    
   //Texture
   float4 specularColour = txSpecular.Sample(samLinear,1.0 - input.Tex);

   float4 textureColour = txDiffuse.Sample(samLinear, 1.0 - input.Tex);
   float4 earthNight = txNightDiffuse.Sample(samLinear, 1.0 - input.Tex);

 
 

    float3 normalW = mul(float4(input.Norm, 0.0f), World).xyz;

    normalW = normalize(normalW);

    float3 toEye = normalize(EyePosW - input.PosW);


    //Ambient
 //   float3 ambient = AmbientMtrl * AmbientLight;
   
    
    //Specular
    float3 r = reflect(-LightVecW, normalW); 
   // float3 r = reflect(-toEye, normalW); ///dw 
    float specularAmount = pow(max(dot(r, toEye), 0.0f), SpecularPower);
    float3 specularValue = specularColour.xyz;

   

    //float4 endColour;
   
    // Compute Colour using Diffuse and ambient lighting
   // float diffuseAmount = max(dot(LightVecW, normalW), 0.0f);

   // float3 diffuse = diffuseAmount * (DiffuseMtrl * DiffuseLight).rgb;

    float3 specular;


    if (specularColour.x > 0)
    {
        specular = specularColour.rgb;
    }
    else
    {
        specular = specularAmount * (specularValue * SpecularMtrl * SpecularLight).rgb;
    }
    

    //specular = specularAmount * (specularValue * SpecularMtrl * SpecularLight).rgb;
    
    float4 amb = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 dif = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

    float4 A, D, S;



    ProcessPointLight(material, pointLight, input.PosW, normalW, toEye, A, D, S);
    amb += A;
    dif += D;
    spec += S;

    ProcessSpotLight(material, spotLight, input.PosW, normalW, toEye, A, D, S);
    amb += A;
    dif += D;
    spec += S;

    //Day night cycle
    float3 colour_day = float3(1.0f, 1.0f, 1.0f);
    colour_day *= (textureColour.rgb);

    float3 colour_night = float3(1.0f, 1.0f, 1.0f);
    colour_night *= (earthNight.rgb);

    //This creates earths day/night cycle, when there is no light on the face the night texture is used
    //then when the sun rises, the night lights go out
    float3 dayNightProcess = lerp(colour_night, colour_day ,(dif + 1.0) / 2.0);

   

   
 
    
    float4 endColour;
    //endColour.rgb = amb + dif + (specularColour * spec) + textureColour;
    //endColour.rgb = ((textureColour * (diffuse + ambient)) + specular);
    //endColour.rgb =  ((ambient + diffuse) + (specular)) + textureColour;
    endColour.rgb = dayNightProcess * (amb + dif) + (specularColour * spec);
    endColour.a = material.Diffuse.a;

    return endColour;
}
