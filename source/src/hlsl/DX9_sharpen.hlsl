    Texture2D theTexture : register(t0);
    float2 textureSize : register(c0);
    SamplerState theSampler = sampler_state
    {
        addressU = Clamp;
        addressV = Clamp;
        mipfilter = NONE;
        minfilter = LINEAR;
        magfilter = LINEAR;
    };

    struct PixelShaderInput
    {
        float4 pos : SV_POSITION;
        float2 tex : TEXCOORD0;
        float4 color : COLOR0;
    };

    #define sharp_clamp ( 0.050000 )
    #define sharp_strength ( 0.500000 )
    #define CoefLuma float3(0.2126, 0.7152, 0.0722)

    float4 main(PixelShaderInput input) : SV_TARGET
    {
        float3 origset = theTexture.Sample(theSampler, input.tex).rgb;
        float3 sharp_strength_luma = (CoefLuma * sharp_strength);
        float2 textureSize2 = float2(textureSize.x, -textureSize.y);
        float3 blur_sample;
        blur_sample  = theTexture.Sample(theSampler, input.tex + textureSize2).rgb;
        blur_sample += theTexture.Sample(theSampler, input.tex - textureSize).rgb;
        blur_sample += theTexture.Sample(theSampler, input.tex + textureSize).rgb;
        blur_sample += theTexture.Sample(theSampler, input.tex - textureSize2).rgb;
        blur_sample *= 0.25;
        float3 sharp = origset - blur_sample;
        float4 sharp_strength_luma_clamp = float4(sharp_strength_luma * (0.5 / sharp_clamp), 0.5);
        float sharp_luma = saturate(dot(float4(sharp,1.0), sharp_strength_luma_clamp));
        sharp_luma = (sharp_clamp * 2.0) * sharp_luma - sharp_clamp;
        float3 outputcolor = origset + sharp_luma;
        return float4(saturate(outputcolor), 1.0);
    }