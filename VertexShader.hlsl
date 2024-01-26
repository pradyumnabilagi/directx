struct VSOut
{
	float3 color: Color;
	float4 pos : SV_Position;
};

cbuffer CBuf {
	matrix transform;
};

VSOut main(float4 pos : Position,float3 color : Color)
{
	VSOut vso;
	vso.pos = mul(pos,transform);
	vso.color = color;
	return vso;
}