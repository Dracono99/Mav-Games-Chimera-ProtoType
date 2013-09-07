uniform matrix gWorldInverse;
uniform matrix gWorldTransformMatrix;
uniform matrix ViewProjMatrix;
uniform matrix InstanceViewProjectionMatrix;
uniform matrix DecalWVP;
uniform vector LightDirection;
uniform matrix InstanceTranslationVector;
uniform extern float specPower;
uniform extern vector camPos;
uniform extern texture gTex;
uniform extern texture gTexNorm;
uniform extern texture gRttTex;
uniform extern texture gTexSpec;
uniform extern texture gVertDispMagTex;
uniform extern texture gVertNormQuatTex;
uniform extern texture gDamagedTex;
uniform extern texture gUndamagedTex;
uniform extern float InverseViewportWidth;
uniform extern float InverseViewportHeight;
uniform extern int postProcessEffect;// 0 for blur , 1 for edge enhance, 2 for sharp image
uniform extern int UseDmgMaps; // 0 for no 1 for yes  use dmg maps if its already damaged but not for the first time its being damaged
uniform extern float4 gDamageSphere; // xyz are sphere pos in object space w is radius of damage sphere
// note to self vertnormquat needs to be converted from -1.0f to 1.0f to 0.0f to 1.0f

sampler TexDiff = sampler_state
{
	Texture = <gTex>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};
sampler DmgdTex = sampler_state
{
	Texture = <gDamagedTex>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU = CLAMP;
	AddressV = CLAMP;
};
sampler UnDmgdTex = sampler_state
{
	Texture = <gUndamagedTex>;
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

sampler VertexDispMagTex = sampler_state
{
	Texture = <gVertDispMagTex>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = NONE;
	AddressU = CLAMP;
	AddressV = CLAMP;
};

sampler VertexNormQuatTex = sampler_state
{
	Texture = <gVertNormQuatTex>;
	MinFilter = POINT;
	MagFilter = POINT;
	MipFilter = NONE;
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
static float3 DS_PlanePoint = {0.0f,0.3f,0.0f};
static float3 DS_PlaneNormal = {0.0f,-1.0f,0.0f};
static float FogStart = 0.0f;
static float FogEnd = 2000.0f;
static float3 BLACK = {0.0f,0.0f,0.0f};
static float MeshDensity = 20.0f;
static float DamageFactor = 2.0f;// damage factor is multiplied times the distance from the damage sphere center in short it reduces the amount of displace ment based on the factor
float4 QuatMultiply(in float4 quat1, in float4 quat2)
{
	float nw = ((quat1.w*quat2.w)-(quat1.x*quat2.x)-(quat1.y*quat2.y)-(quat1.z*quat2.z));
	float nx = ((quat1.w*quat2.x)+(quat1.x*quat2.w)-(quat1.y*quat2.z)-(quat1.z*quat2.y));
	float ny = ((quat1.w*quat2.y)-(quat1.x*quat2.z)+(quat1.y*quat2.w)-(quat1.z*quat2.x));
	float nz = ((quat1.w*quat2.z)+(quat1.x*quat2.y)-(quat1.y*quat2.x)+(quat1.z*quat2.w));
	return float4(nx,ny,nz,nw);
}
float3 GetForwardVectorFromQuat(in float4 quat)
{
	return float3((2.0f*(quat.x*quat.z+quat.w*quat.y)),(2.0f*(quat.y*quat.z-quat.w*quat.x)),(1.0f-2.0f*(quat.x*quat.x+quat.y*quat.y)));
}
float3 GetUpVectorFromQuat(in float4 quat)
{
	return float3((2.0f*(quat.x*quat.y-quat.w*quat.z)),(1.0f-2.0f*(quat.x*quat.x+quat.z*quat.z)),(2.0f*(quat.y*quat.z+quat.w*quat.x)));
}
float3 GetRightVectorFromQuat(in float4 quat)
{
	return float3((1.0f-2.0f*(quat.y*quat.y+quat.z*quat.z)),(2.0f*(quat.x+quat.y+quat.w*quat.z)),(2.0f*(quat.x*quat.z-quat.w*quat.y)));
}
float3 GetDirectionFromFirstToSecond(in float3 first,in float3 second)
{
	float3 dir = second - first;
	return normalize(dir);
}
float3 GetZedToOneFromNegOneToOne(in float3 norm)
{
	norm = normalize(norm);
	float3 res;
	res.x = 0.5f*norm.x+0.5f;
	res.y = 0.5f*norm.y+0.5f;
	res.z = 0.5f*norm.z+0.5f;
	return normalize(res);
}
float3 GetNegOneToOneFromZedToOne(in float3 norm)
{
	norm = normalize(norm);
	float3 res;
	res.x = norm.x*2.0f-1.0f;
	res.y = norm.y*2.0f-1.0f;
	res.z = norm.z*2.0f-1.0f;
	return normalize(res);
}
float GetDistanceFromFirstToSecond(in float3 first,in float3 second)
{
	float3 dir = second - first;
	return length(dir);
}
float2 FixRTTthingy(in float2 texcoord)
{
	float2 output;
	float px = (texcoord.x*2.0000000001f)-1.000000000001f;
	float py = (texcoord.y*-2.0000000001f)+1.000000000001f;
	output.x=0.500000000*(1.000000000+px-0.00048828125);
	output.y=0.500000000*(1.000000000-py-0.00048828125);
	return output;
}
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
struct InstanceVS_Input
{
	float3 position : POSITION0;
	float2 idx : TEXCOORD0;
};
struct InstanceVS_Output
{
	float4 position : POSITION0;
};
struct DmgSdrVrtNrmQVS_INPUT
{
	float3 position : POSITION0;
	float3 tangent : TANGENT0;
	float3 binormal : BINORMAL0;
	float3 normal : NORMAL0;
	float2 texcoord : TEXCOORD0;
};
struct DmgSdrVrtNrmQVS_OUTPUT
{
	float4 position : POSITION0;
	float4 VertNormQuat : COLOR0;
};
struct DmgSdrVrtMagVS_OUTPUT
{
	float4 position : POSITION0;
	float4 vertDspMag : COLOR0;
	float2 texcoord : TEXCOORD0;
}; 
struct DmgSdrComboTexVS_OUTPUT 
{
	float4 position : POSITION0;
	float4 TexChoice : COLOR0;	// color just determines which texture we use greater than .5 = undamaged < .5 damaged assigned as 0 or 1
	float2 texcoord : TEXCOORD0;
};
struct DmgSdrVrtDisMagVS_INPUT
{
	float3 position : POSITION0;
	float3 tangent : TANGENT0;
	float3 binormal : BINORMAL0;
	float3 normal : NORMAL0;
	float2 texcoord : TEXCOORD0;
};
struct DmgSdrTexComboVS_INPUT
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
	float4 debugColor : COLOR0;
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
InstanceVS_Output InstanceVS(InstanceVS_Input input)
{
	InstanceVS_Output output = (InstanceVS_Output)0;
	//float3 posW = mul(input.position,InstanceTranslationVector).xyz;
	output.position = mul(float4(input.position,1.0f),InstanceViewProjectionMatrix);
	return output;	
}
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

DmgSdrVrtNrmQVS_OUTPUT DmgSdrVertNormVS(DmgSdrVrtNrmQVS_INPUT input)
{
// zero out all members of the output instance.
	DmgSdrVrtNrmQVS_OUTPUT output = (DmgSdrVrtNrmQVS_OUTPUT)0;
	if(UseDmgMaps==0)// this is the first time its been damaged dont refference maps
	{
		float dist = DamageFactor * GetDistanceFromFirstToSecond(input.position,float3(gDamageSphere.xyz));
		if(dist<gDamageSphere.w)
		{
			float3 vertNormDir = GetDirectionFromFirstToSecond(input.position,float3(gDamageSphere.xyz));
			vertNormDir = input.normal;//+vertNormDir;
			vertNormDir = normalize(vertNormDir);
			float px = ((input.texcoord.x*2.000f)-1.000f)-0.00048828828125f;
			float py = ((input.texcoord.y*-2.000f)+1.000f)+0.00048828828125f;
			output.position = float4(px,py,0.0f,1.0f);
			output.VertNormQuat = float4(GetZedToOneFromNegOneToOne(input.normal),0.0f);
			return output;
		}
		else
		{
			float px = ((input.texcoord.x*2.000f)-1.000f)-0.00048828828125f;
			float py = ((input.texcoord.y*-2.000f)+1.000f)+0.00048828828125f;
			output.position = float4(px,py,0.0f,1.0f);
			output.VertNormQuat = float4(GetZedToOneFromNegOneToOne(input.normal),0.0f);
			return output;
		}
	}
	else
	{
		float dist = DamageFactor * GetDistanceFromFirstToSecond(input.position,float3(gDamageSphere.xyz));
		if(dist<gDamageSphere.w)
		{
			float4 vertQuat = float4(GetNegOneToOneFromZedToOne(float3(tex2Dlod(VertexNormQuatTex,float4(input.texcoord,0.0f,0.0f)).xyz)),0.0f);
			float3 vertNorm = GetForwardVectorFromQuat(vertQuat);
			vertNorm = normalize(vertNorm);
			float3 vertNormDir = GetDirectionFromFirstToSecond(input.position,float3(gDamageSphere.xyz));
			vertNormDir = vertNorm;//-vertNormDir;
			vertNormDir = normalize(vertNormDir);
			float px = ((input.texcoord.x*2.000f)-1.000f)-0.00048828828125f;
			float py = ((input.texcoord.y*-2.000f)+1.000f)+0.00048828828125f;
			output.position = float4(px,py,0.0f,1.0f);
			output.VertNormQuat = float4(GetZedToOneFromNegOneToOne(vertNormDir),1.0f);
			return output;
		}
		else
		{
			float4 vertQuat = float4(GetNegOneToOneFromZedToOne(float3(tex2Dlod(VertexNormQuatTex,float4(input.texcoord,0.0f,0.0f)).xyz)),0.0f);
			float3 vertNorm = GetForwardVectorFromQuat(vertQuat);	
			vertNorm = normalize(vertNorm);
			float px = ((input.texcoord.x*2.000f)-1.000f)-0.00048828828125f;
			float py = ((input.texcoord.y*-2.000f)+1.000f)+0.00048828828125f;
			output.position = float4(px,py,0.0f,1.0f);
			output.VertNormQuat = float4(GetZedToOneFromNegOneToOne(vertNorm),1.0f);
			return output;		
		}
	}	
}
float4 DmgSdrVertNormPS(float4 VertNormQuat : COLOR0): COLOR0
{
	float4 clr;
	clr.xyzw=VertNormQuat.rgba;
	return clr;
}
DmgSdrVrtMagVS_OUTPUT DmgSdrVertMagVS(DmgSdrVrtDisMagVS_INPUT input)
{
	// zero out all members of the output instance.
	DmgSdrVrtMagVS_OUTPUT output = (DmgSdrVrtMagVS_OUTPUT)0;
	if(UseDmgMaps==0)// this is the first time its been damaged dont refference maps
	{
		float dist = DamageFactor * GetDistanceFromFirstToSecond(input.position,float3(gDamageSphere.xyz));
		if(dist<gDamageSphere.w)
		{
			if(dist<1)
			{
				dist=1;
			}
			dist=dist*DamageFactor;
			float dmg = MeshDensity/dist;
			dmg = dmg / MeshDensity;
			float px = ((input.texcoord.x*2.000f)-1.000f)-0.00048828828125f;
			float py = ((input.texcoord.y*-2.000f)+1.000f)+0.00048828828125f;
			output.position = float4(px,py,0.0f,1.0f);
			output.vertDspMag = float4(0.0f,0.0f,0.0f,dmg);
			//output.texcoord.x=0.500000000*(1.000000000+output.position.x-0.00048828125);
			//output.texcoord.y=0.500000000*(1.000000000-output.position.y-0.00048828125);
			output.texcoord.x=input.texcoord.x;
			output.texcoord.y=input.texcoord.y;
			return output;
		}
		else
		{
			float px = ((input.texcoord.x*2.000f)-1.000f)-0.00048828828125f;
			float py = ((input.texcoord.y*-2.000f)+1.000f)+0.00048828828125f;
			output.position = float4(px,py,0.0f,1.0f);
			output.vertDspMag = float4(0.0f,0.0f,0.0f,0.0f);
			//output.texcoord.x=0.500000000*(1.000000000+output.position.x-0.00048828125);
			//output.texcoord.y=0.500000000*(1.000000000-output.position.y-0.00048828125);
			output.texcoord.x=input.texcoord.x;
			output.texcoord.y=input.texcoord.y;
			return output;
		}
	}
	else
	{
		float dist = DamageFactor * GetDistanceFromFirstToSecond(input.position,float3(gDamageSphere.xyz));
		if(dist<gDamageSphere.w)
		{
			if(dist<1)
			{
				dist=1;
			}
			dist=dist*DamageFactor;
			float dmg = MeshDensity/dist;
			dmg = dmg / MeshDensity;
			//float4 damageRefMap = tex2Dlod(VertexDispMagTex,float4(input.texcoord.x-0.998047f,input.texcoord.y-0.998047f,0.0f,0.0f));
			//dmg=dmg+damageRefMap.w;
			if(dmg>1.0)
			{
				dmg=1.0f;
			}
			float px = ((input.texcoord.x*2.000f)-1.000f)-0.00048828828125f;
			float py = ((input.texcoord.y*-2.000f)+1.000f)+0.00048828828125f;
			output.position = float4(px,py,0.0f,1.0f);
			output.vertDspMag = float4(0.0f,0.0f,0.0f,dmg);
			//output.texcoord.x=0.500000000*(1.000000000+output.position.x-0.00048828125);
			//output.texcoord.y=0.500000000*(1.000000000-output.position.y-0.00048828125);
			output.texcoord.x=input.texcoord.x;
			output.texcoord.y=input.texcoord.y;
			return output;
		}
		else
		{			
			float px = ((input.texcoord.x*2.000f)-1.000f)-0.00048828828125f;
			float py = ((input.texcoord.y*-2.000f)+1.000f)+0.00048828828125f;
			output.position = float4(px,py,0.0f,1.0f);
			output.vertDspMag = float4(0.0f,0.0f,0.0f,0.0f);//tex2Dlod(VertexDispMagTex,float4(input.texcoord.x+0.000f,input.texcoord.y+0.000f,0.0f,0.0f));
			//output.texcoord.x=0.500000000*(1.000000000+output.position.x-0.00048828125);
			//output.texcoord.y=0.500000000*(1.000000000-output.position.y-0.00048828125);
			output.texcoord.x=input.texcoord.x;
			output.texcoord.y=input.texcoord.y;
			return output;		
		}
	}	
}
float4 DmgSdrVertMagPS(float4 VertDspMag : COLOR0, float2 texcoord : TEXCOORD0): COLOR0
{
	float4 clrm;
	float mag = (VertDspMag.a+tex2D(VertexDispMagTex,texcoord).a);
	if(mag>1.0f)
	{
		mag=1.0f;
	}
	clrm.xyzw=mag;
	//clrm=clrm+tex2D(VertexDispMagTex,texcoord);
	//float4 col = VertDspMag;
	//for(int i = 0;i<4;i++)
  	//	 {
    	//		col+=BlurSample[i].w*tex2D(VertexDispMagTex,texcoord+
   	//		float2(BlurSample[i].x*0.000244f,
   	//		BlurSample[i].y*0.000244f));
   	//	 }
	//return col;
	//float4 clrm;
	//clrm.xyzw=VertDspMag.a;
	//if(VertDspMag.a>0.0f)
	//{
		return clrm;
	//}
	//else
	//{
		//return tex2D(VertexDispMagTex,texcoord);
	//}
}
DmgSdrComboTexVS_OUTPUT DmgSdrTexMapComboVS(DmgSdrTexComboVS_INPUT input)
{
	// zero out all members of the output instance.
	DmgSdrComboTexVS_OUTPUT output = (DmgSdrComboTexVS_OUTPUT)0;
	float4 mag = tex2Dlod(VertexDispMagTex,float4(input.texcoord.x,input.texcoord.y,0.0f,0.0f));
	if(mag.w>0.0f)
	{
		output.TexChoice.x=1.0f;
	}
	else
	{
		output.TexChoice.x=0.0f;
	}	
	float px = ((input.texcoord.x*2.000f)-1.000f)-0.00048828828125f;
	float py = ((input.texcoord.y*-2.000f)+1.000f)+0.00048828828125f;
	output.position = float4(px,py,0.0f,1.0f);
	output.texcoord = input.texcoord;
	return output;	
}
float4 DmgSdrTexMapComboPS(float4 TexChoice : COLOR0,float2 texcoord : TEXCOORD0): COLOR0
{
	//return tex2D(DmgdTex,texcoord);
	if(TexChoice.x>=0.5f)
	{
		return float4(tex2D(DmgdTex,texcoord).rgb,1.0f);
	}
	else
	{
		return float4(tex2D(UnDmgdTex,texcoord).rgb,1.0f);
	}	
}
VS_OUTPUT DamageShaderFinalRenderVS(VS_INPUT input)
{
	// zero out all members of the output instance.
	VS_OUTPUT output = (VS_OUTPUT)0;
	float3x3 TBN;
	float4 vertNormalQuat = float4(GetNegOneToOneFromZedToOne(float3(tex2Dlod(VertexNormQuatTex,float4(input.texcoord,0.0f,0.0f)).xyz)),0.0f);
	float4 vertMag = tex2Dlod(VertexDispMagTex,float4(input.texcoord.x,input.texcoord.y,0.0f,0.0f));
	output.debugColor = vertMag;
	//TBN[0]=GetUpVectorFromQuat(vertNormalQuat);
	//TBN[1]=GetRightVectorFromQuat(vertNormalQuat);
	//TBN[2]=GetForwardVectorFromQuat(vertNormalQuat);
//float3x3 TBN;
	TBN[0]=input.tangent;
	TBN[1]=input.binormal;
	TBN[2]=input.normal;
	float3 posL = input.position + (TBN[2] * -1.0f * (MeshDensity* vertMag.w));//(GetForwardVectorFromQuat(vertNormalQuat)

	float3x3 toTangentSpace = transpose(TBN);

	float3 eyePosL = mul(camPos,gWorldInverse).xyz;

	float3 toEyeL = eyePosL - posL;

	output.toEyeT = mul(toEyeL,toTangentSpace);

	float3 lightDirL = mul(LightDirection.xyz,gWorldInverse);
	output.lightDirT = mul(lightDirL,toTangentSpace);

	output.position = mul(float4(posL,1.0f), ViewProjMatrix);
	output.pos=output.position;
	output.texcoord = input.texcoord;
	return output;
}
float4 DamageShaderFinalRenderPS(float3 toEyeT : TEXCOORD0, float3 lightDirT : TEXCOORD1, float2 texcoord : TEXCOORD2,float4 pos : TEXCOORD3, float4 debugColor : COLOR0) : COLOR
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
	//return float4(spec.rgb,1.0f);
	float3 hi= tex2D(VertexDispMagTex,texcoord).rgb;
	return float4(color,1.0f);
	//return float4(debugColor.rgb,1.0f);
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
	output.position.x = (input.texcoord.x*2.001f)-1.001f;//-InverseViewportWidth));//+InverseViewportWidth;
	//if(output.position.x>0)
	//{
	//	output.position.x=output.position.x+InverseViewportWidth;
	//}
	//else if(output.position.x<0)
	//{
	//	output.position.x=output.position.x-(InverseViewportWidth*2.0f);
	//}
	output.position.y = (input.texcoord.y*-2.001f)+1.001f;//+InverseViewportHeight;
	//output.position.x=input.position.x;
	//output.position.y=input.position.y;
	output.position.w=1;
	output.position.z=0;
	output.texcoord.x=input.texcoord.x;//-InverseViewportWidth;//*1.0f-InverseViewportWidth;
	output.texcoord.y=input.texcoord.y;//*1.0f-InverseViewportHeight;
	//output.texcoord.x=0.5*(1+input.position.x-InverseViewportWidth);
	//output.texcoord.y=0.5*(1-input.position.y-InverseViewportHeight);
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
technique DmgShaderVertNormQuat
{
	pass P0
	{
		vertexShader = compile vs_3_0 DmgSdrVertNormVS();
		pixelShader = compile ps_3_0 DmgSdrVertNormPS();	
		AlphaBlendEnable = false;			
	}
}
technique InstanceQuads
{
	pass P0
	{
		vertexShader = compile vs_3_0 InstanceVS();
		pixelShader = compile ps_3_0 DynamicShadowPS();
	}
}
technique DmgShaderVertDspMag
{
	pass P0
	{
		vertexShader = compile vs_3_0 DmgSdrVertMagVS();
		pixelShader = compile ps_3_0 DmgSdrVertMagPS();	
		AlphaBlendEnable = false;
		//SrcBlend = One;
		//DestBlend = zero; 
		//BlendOpAlpha = add;		  			
	}
}
technique DmgShaderTexMapBlend
{
	pass P0
	{
		vertexShader = compile vs_3_0 DmgSdrTexMapComboVS();
		pixelShader = compile ps_3_0 DmgSdrTexMapComboPS();	
		AlphaBlendEnable = true;
    		SrcBlend = One;
		DestBlend = zero; 	
	}
}
technique DmgShaderFinalRender
{
	pass P0
	{
		vertexShader = compile vs_3_0 DamageShaderFinalRenderVS();
		pixelShader = compile ps_3_0 DamageShaderFinalRenderPS();	
		AlphaBlendEnable = true;
		SrcBlend = srcalpha;
		DestBlend = invsrcalpha;	
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