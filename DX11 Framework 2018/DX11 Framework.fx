//--------------------------------------------------------------------------------------
// File: DX11 Framework.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

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

}

//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    //float4 Color : COLOR0;
    float3 Norm: NORMAL;
   // float3 PosW : POSITION;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

VS_OUTPUT VS(float4 Pos : POSITION, float3 NormalL : NORMAL)
{
    
    VS_OUTPUT output = (VS_OUTPUT)0;
     output.Norm = NormalL;

    output.Pos = mul( Pos, World );
    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );




    return output;

}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUTPUT input ) : SV_Target
{
     // Convert from local space to world space 
    // W component of vector is 0 as vectors cannot be translated
    float3 normalW = mul(float4(input.Norm, 0.0f), World).xyz;
    normalW = normalize(normalW);

    float4 endColour;
    // Compute Colour using Diffuse lighting only
    float diffuseAmount = max(dot(LightVecW, normalW), 0.0f);
    endColour.rgb = diffuseAmount * (DiffuseMtrl * DiffuseLight).rgb;
    endColour.a = DiffuseMtrl.a;

    return endColour;
}
