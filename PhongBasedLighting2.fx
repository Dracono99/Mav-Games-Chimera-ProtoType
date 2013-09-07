uniform matrix gWorldInverse;
uniform matrix gWorldTransformMatrix;
uniform matrix ViewProjMatrix;
uniform matrix DecalWVP;
uniform vector LightDirection;
uniform extern float specPower;
uniform extern vector camPos;
uniform extern texture gTex;
uniform extern texture gTexNorm;
uniform extern texture gRttTex;
uniform extern texture gTexSpec;
uniform extern float InverseViewportWidth;
uniform extern float InverseViewportHeight;
uniform extern int postProcessEffect;// 0 for blur , 1 for edge enhance, 2 for sharp image

sampler TexDiff = sampler_state
{
	Texture = <gTex>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU = CLAMP;
	AddressV = CLAMP;
};

sampler TexNorm = sampler_state
{
	Texture = <gTexNorm>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU = CLAMP;
	AddressV = CLAMP;
};

sampler TexSpec = sampler_state
{
	Texture = <gTexSpec>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU = CLAMP;
	AddressV = CLAMP;
};

sampler RenderTarget = sampler_state
{
	Texture = <gRttTex>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU = CLAMP;
	AddressV = CLAMP;
};
//
// Global variables used to hold the ambient light intensity (ambient
// light the light source emits) and the diffuse light
// intensity (diffuse light the light source emits). These
// variables are initialized here in the shader.
//
static const float4 BlurSample[4]={
-1.0, 0.0, 0, 0.25,
1.0, 0.0, 0, 0.25,
0.0, 1.0, 0, 0.25,
0.0, -1.0, 0, 0.25
};

static const float4 EdgeEnhanceSample[6] = {
   -1.0,   1.0,   0,   1.0,
   0.0,   1.0,   0,   2.0,
   1.0,   1.0,   0,   1.0,
   -1.0,   -1.0,   0,   -1.0,
   0.0,   -1.0,   0,   -2.0,
   1.0,   -1.0,   0,   -1.0
};

static const float4 ImageSharpenSample[5] = {
   0.0,   0.0,   0,   11.0/3.0,
   0.0,   1.0,   0,   -2.0/3.0,
   0.0,   -1.0,   0,   -2.0/3.0,
   -1.0,   0.0,   0,   -2.0/3.0,
   1.0,   0.0,   0,   -2.0/3.0
   };
static vector DiffuseLightIntensity = {0.5f, 0.5f, 0.5f, 1.0f};
static vector AmbientLightIntensity = {0.1f, 0.1f, 0.1f, 0.9f};
static vector AttenuationFactors = {0.0f,0.01f,0.0f,0.0f};
static float3 DS_PlanePoint = {0.0f,0.0f,0.0f};
static float3 DS_PlaneNormal = {0.0f,-1.0f,0.0f};
static float FogStart = 0.0f;
static float FogEnd = 2000.0f;
static float3 BLACK = {0.0f,0.0f,0.0f};
//
// Input and Output structures.
//
struct VS_INPUT
{
	float3 position : POSITION0;
	float3 tangent : TANGENT0;
	float3 binormal : BINORMAL0;
	float3 normal : NORMAL0;
	float2 texcoord : TEXCOORD0;
};
struct DS_VS_Input
{
	float3 position : POSITION0;
	float3 tangent : TANGENT0;
	float3 binormal : BINORMAL0;
	float3 normal : NORMAL0;
	float2 texcoord : TEXCOORD0;
};
struct DS_VS_Output
{
	float4 pos : POSITION0;
};
struct ppvsInput
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 texcoord : TEXCOORD;
};
struct VS_OUTPUT
{
	float4 position : POSITION;
	float3 toEyeT : TEXCOORD0;
	float3 lightDirT : TEXCOORD1;
	float2 texcoord : TEXCOORD2;
	float4 pos : TEXCOORD3;
};
struct DecalVS_OUTPUT
{
	float4 position : POSITION;
	float3 toEyeT : TEXCOORD0;
	float3 lightDirT : TEXCOORD1;
	float4 texcoord : TEXCOORD2;
};
struct ppvsOutput
{
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
};
//
// Main
//
DS_VS_Output DynamicShadowVS(DS_VS_Input input)
{
	DS_VS_Output output = (DS_VS_Output)0;
	float3 posW = mul(float4(input.position,1.0f),gWorldTransformMatrix).xyz;
	//float3 lightDirection = posW - camPos;
	//lightDirection = normalize(lightDirection);
	float t = dot(DS_PlaneNormal,(DS_PlanePoint-posW));
	t = t / dot(DS_PlaneNormal,LightDirection.xyz);
	posW = posW + t*LightDirection.xyz;
	output.pos = mul(float4(posW,1.0f),ViewProjMatrix);
	return output;
};
float4 DynamicShadowPS(float4 pos : POSITION0) : COLOR
{
	return float4(0.0f,0.0f,0.0f,1.0f);
}
VS_OUTPUT ColorVS(VS_INPUT input)
{
// zero out all members of the output instance.
	VS_OUTPUT output = (VS_OUTPUT)0;
	float3x3 TBN;
	TBN[0]=input.tangent;
	TBN[1]=input.binormal;
	TBN[2]=input.normal;

	float3x3 toTangentSpace = transpose(TBN);

	float3 eyePosL = mul(camPos,gWorldInverse).xyz;

	float3 toEyeL = eyePosL - input.position;

	output.toEyeT = mul(toEyeL,toTangentSpace);

	float3 lightDirL = mul(LightDirection.xyz,gWorldInverse);
	output.lightDirT = mul(lightDirL,toTangentSpace);

	output.position = mul(float4(input.position,1.0f), ViewProjMatrix);
	output.pos=output.position;
	output.texcoord = input.texcoord;
	return output;
}

DecalVS_OUTPUT DecalVS(VS_INPUT input)
{
// zero out all members of the output instance.
	DecalVS_OUTPUT output = (DecalVS_OUTPUT)0;
	float3x3 TBN;
	TBN[0]=input.tangent;
	TBN[1]=input.binormal;
	TBN[2]=input.normal;

	float3x3 toTangentSpace = transpose(TBN);

	float3 eyePosL = mul(camPos,gWorldInverse).xyz;

	float3 toEyeL = eyePosL - input.position;

	output.toEyeT = mul(toEyeL,toTangentSpace);

	float3 lightDirL = mul(LightDirection.xyz,gWorldInverse);
	output.lightDirT = mul(lightDirL,toTangentSpace);

	output.position = mul(float4(input.position,1.0f), ViewProjMatrix);

	output.texcoord = mul(float4(input.position,1.0f),DecalWVP);
	return output;
}
ppvsOutput ImposterVS(ppvsInput input)
{
	ppvsOutput output = (ppvsOutput)0;
	output.position = mul(float4(input.position,1.0f), ViewProjMatrix);
	output.texcoord = input.texcoord;
	return output;
}
float4 ImposterPS(float2 texcoord : TEXCOORD0):COLOR
{
	float4 color = tex2D(TexDiff,texcoord);
	if((color.r==0.0f)&&(color.g==0.0f)&&(color.b==0.0f))
	{
		color.a=0.0f;
	}
	return color;
}

ppvsOutput PostProcessVS(ppvsInput input)
{
	ppvsOutput output = (ppvsOutput)0;
	output.position.x=input.position.x;
	output.position.y=input.position.y;
	output.position.w=1;
	output.position.z=0;
	output.texcoord.x=0.5*(1+input.position.x-InverseViewportWidth);
	output.texcoord.y=0.5*(1-input.position.y-InverseViewportHeight);
	return output;
}

float4 ColorPS(float3 toEyeT : TEXCOORD0, float3 lightDirT : TEXCOORD1, float2 texcoord : TEXCOORD2,float4 pos : TEXCOORD3) : COLOR
{
	float pz = pos.z;
	float fg = FogEnd - pz;
	fg=fg/FogEnd-FogStart;
	if(fg<0)
	{
		fg=0;
	}
	else if(fg>1)
	{
		fg=1;
	}	
	float3 toEyeTp = normalize(toEyeT);
	float3 lightDirTp = normalize(lightDirT);
	float3 lightVecT = -lightDirTp;
	float3 normalT = tex2D(TexNorm,texcoord);
	normalT=2.0f*normalT-1.0f;
	normalT=normalize(normalT);
	float3 r = reflect(-lightVecT,normalT);
	float t = pow(max(dot(r,toEyeTp),0.0f),specPower);
	float s = max(dot(lightVecT,normalT),0.0f);
	if(s<=0.0f)
	t=0.0f;
	float3 spec = t*(tex2D(TexSpec,texcoord)*tex2D(TexSpec,texcoord)).rgb;
	float3 diff = s*(tex2D(TexDiff,texcoord)*DiffuseLightIntensity).rgb;
	float3 amb = (tex2D(TexDiff,texcoord)*AmbientLightIntensity).rgb;	
	float4 texColor = tex2D(TexDiff,texcoord);
	float3 color = (amb+diff)+spec;
	//color = fg*color + (1-fg)*BLACK;
	return float4(color,texColor.a);
}

float4 DecalColorPS(float3 toEyeT : TEXCOORD0, float3 lightDirT : TEXCOORD1, float4 texcoord : TEXCOORD2) : COLOR
{
	// Project the texture coords and scale/offset to [0, 1].
	float4 projTex=texcoord;
        projTex.xyz /= projTex.w;
        projTex.x =  0.5f*projTex.x + 0.5f;
	projTex.y = -0.5f*projTex.y + 0.5f;
	float3 toEyeTp = normalize(toEyeT);
	float3 lightDirTp = normalize(lightDirT);
	float3 lightVecT = -lightDirTp;
	float3 normalT = tex2D(TexNorm,projTex.xy);
	normalT=2.0f*normalT-1.0f;
	normalT=normalize(normalT);
	float3 r = reflect(-lightVecT,normalT);
	float t = pow(max(dot(r,toEyeTp),0.0f),specPower);
	float s = max(dot(lightVecT,normalT),0.0f);
	if(s<=0.0f)
	t=0.0f;
	float3 spec = t*(tex2D(TexSpec,projTex.xy)*tex2D(TexSpec,projTex.xy)).rgb;
	float3 diff = s*(tex2D(TexDiff,projTex.xy)*DiffuseLightIntensity).rgb;
	float3 amb = (tex2D(TexDiff,projTex.xy)*AmbientLightIntensity).rgb;	
	float4 texColor = tex2D(TexDiff,projTex.xy);
	float3 color = (amb+diff)+spec;
	float4 retValue = float4(color,texColor.a);
	if(projTex.x>1.0f||projTex.x<0.0f)
	{
		retValue.a=0.0f;
		return retValue;
	}
	else if(projTex.y>1.0f||projTex.y<0.0f)
	{
		retValue.a=0.0f;
		return retValue;
	}
	else if(projTex.z>1.0f||projTex.z<0.0f)
	{
		retValue.a=0.0f;
		return retValue;
	}
	else
	{
		return retValue;
	}
}

float4 PostProcessPS(float2 texcoord: TEXCOORD):COLOR
{
	float4 col = float4(0,0,0,0);

	if(postProcessEffect==0)
	{
		 for(int i = 0;i<4;i++)
  		 {
    			col+=BlurSample[i].w*tex2D(RenderTarget,texcoord+
   			float2(BlurSample[i].x*InverseViewportWidth,
   			BlurSample[i].y*InverseViewportHeight));
   		 }
	return col;
	}
	else if(postProcessEffect==1)
	{
		 for(int i = 0;i<6;i++)
  		 {
    			col+=EdgeEnhanceSample[i].w*tex2D(RenderTarget,texcoord+
   			float2(EdgeEnhanceSample[i].x*InverseViewportWidth,
   			EdgeEnhanceSample[i].y*InverseViewportHeight));
   		 }
	return col;
	}
	else if(postProcessEffect==2)
	{
		 for(int i = 0;i<5;i++)
  		 {
    			col+=ImageSharpenSample[i].w*tex2D(RenderTarget,texcoord+
   			float2(ImageSharpenSample[i].x*InverseViewportWidth,
   			ImageSharpenSample[i].y*InverseViewportHeight));
   		 }
	return col;
	}
	else
	{
		col = tex2D(RenderTarget,texcoord);
		return col;
	}
}

technique ColorTech
{
	pass P0
	{
		vertexShader = compile vs_3_0 ColorVS();
		pixelShader = compile ps_3_0 ColorPS();

		AlphaBlendEnable = true;
		SrcBlend = srcalpha;
		DestBlend = invsrcalpha;
	}
}
technique DecalProjection
{
	pass P0
	{
		vertexShader = compile vs_3_0 DecalVS();
		pixelShader = compile ps_3_0 DecalColorPS();
		
		AlphaBlendEnable = true;
		SrcBlend = srcalpha;
		DestBlend = invsrcalpha;
		BlendOpAlpha = add;
	}
}

technique PostProcess
{
	pass P0
	{
		vertexShader = compile vs_3_0 PostProcessVS();
		pixelShader = compile ps_3_0 PostProcessPS();
		AlphaBlendEnable = false;
	}
}

technique DynamicShadows
{
	pass p0
	{
		vertexShader = compile vs_3_0 DynamicShadowVS();
		pixelShader = compile ps_3_0 DynamicShadowPS();
	}
}
technique Imposters
{
	pass p0
	{
		vertexShader = compile vs_3_0 ImposterVS();
		pixelShader = compile ps_3_0 ImposterPS(); 
		AlphaBlendEnable = true;
		SrcBlend = srcalpha;
		DestBlend = invsrcalpha;
		BlendOpAlpha = add;
	}
}