   #pragma pack_matrix( row_major )

   cbuffer VertexShaderConstants : register(b0)
   {
       matrix projectionAndView;
   };

   struct VertexShaderInput
   {
       float2 pos : POSITION;
       float2 tex : TEXCOORD0;
       float4 color : COLOR0;
   };

   struct VertexShaderOutput
   {
       float4 pos : SV_POSITION;
       float2 tex : TEXCOORD0;
       float4 color : COLOR0;
   };

   VertexShaderOutput main(VertexShaderInput input)
   {
       VertexShaderOutput output;
       float4 pos = float4(input.pos, 0.0f, 1.0f);

       // Transform the vertex position into projected space.
       pos = mul(pos, projectionAndView);
       output.pos = pos;

       // Pass through texture coordinates and color values without transformation
       output.tex = input.tex;
       output.color = float4(input.color.b, input.color.g, input.color.r, input.color.a);

       return output;
   }