//--------------------------------------------------------------------------------------
// File: DX11 Framework.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------
// Texturing
//-------------------------------------------------------------------------------------

Texture2D txDiffuse : register( t0 );
Texture2D txSpecular : register (t1);
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
    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );
    output.PosW = mul(Pos, World);
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
   float4 textureColour = txDiffuse.Sample(samLinear, input.Tex);
   float4 specularColour = txSpecular.Sample(samLinear, input.Tex);

   float3 specularValue = specularColour.xyz;
   float specularPower = specularColour.w;


    float3 normalW = mul(float4(input.Norm, 0.0f), World).xyz;

    normalW = normalize(normalW);

    float3 toEye = normalize(EyePosW - input.Pos.xyz);

    //Ambient
    float3 ambient = AmbientMtrl * AmbientLight;
    float4 endColour;
    
    //Specular
    float3 r = reflect(-LightVecW, normalW);
    float specularAmount = pow(max(dot(r, toEye), 0.0f), SpecularPower);

   
    // Compute Colour using Diffuse and ambient lighting
    float diffuseAmount = max(dot(LightVecW, normalW), 0.0f);

    float3 diffuse = diffuseAmount * ( textureColour * DiffuseMtrl * DiffuseLight).rgb;
    float3 specular = specularAmount * (specularValue * SpecularMtrl * SpecularLight).rgb;

    endColour.rgb = ambient + diffuse + specular;
    endColour.a = DiffuseMtrl.a;

    return endColour;
}
