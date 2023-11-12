
// 方向光
struct DirectionalLight
{
    float4 LightIntensity;
    float3 Direction;
    float Pad;
};

// 点光
struct PointLight
{
    float4 LightIntensity;

    float3 Position;
    float Range;

    float3 Att;
    float Pad;
};

// 聚光灯
struct SpotLight
{
    float4 LightIntensity;

    float3 Position;
    float Range;

    float3 Direction;
    float Spot;

    float3 Att;
    float Pad;
};

// 物体表面材质
struct Material
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular; // w = SpecPower
    float4 Reflect;
};



void ComputeDirectionalLight(Material mat, DirectionalLight L,
	float3 normal, float3 toEye,
	out float4 ambient,
	out float4 diffuse,
	out float4 spec)
{
	// 初始化输出
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// 光向量与照射方向相反
    float3 lightVec = -L.Direction;

	// 添加环境光
    ambient = mat.Ambient * L.LightIntensity;

	// 添加漫反射光和镜面光
    float diffuseFactor = dot(lightVec, normal);

	// 展开，避免动态分支
	[flatten]
    if (diffuseFactor > 0.0f)
    {
        //Blinn--Phong模型
        float3 h = normalize(lightVec + toEye);
        float3 halfdir = normalize(lightVec + normal);
        float specFactor = pow(max(saturate(dot(halfdir, normal)), 0.0f), mat.Specular.w);

        diffuse = diffuseFactor * mat.Diffuse * L.LightIntensity;
        spec = specFactor * mat.Specular * L.LightIntensity;
    }
}


void ComputePointLight(Material mat, PointLight L, float3 pos, float3 normal, float3 toEye,
	out float4 ambient, out float4 diffuse, out float4 spec)
{
	// 初始化输出
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// 从表面到光源的向量
    float3 lightVec = L.Position - pos; //worldlight

	// 表面到光线的距离
    float d = length(lightVec);

	// 灯光范围测试
    if (d > L.Range)
        return;

	// 标准化光向量
    lightVec /= d;

	// 环境光计算
    ambient = mat.Ambient * L.LightIntensity;

	// 漫反射和镜面计算
    float diffuseFactor = dot(lightVec, normal); //diffuse

	// 展开以避免动态分支
	[flatten]
    if (diffuseFactor > 0.0f)
    {
        //Blinn--Phong模型
        float3 h = normalize(lightVec + toEye);
        float3 halfdir = normalize(lightVec + normal);
        float specFactor = pow(max(saturate(dot(halfdir, normal)), 0.0f), mat.Specular.w);

        diffuse = diffuseFactor * mat.Diffuse * L.LightIntensity;
        spec = specFactor * mat.Specular * L.LightIntensity;
    }

	// 光的衰弱
    float att = 1.0f / dot(L.Att, float3(1.0f, d, d * d));

    diffuse *= att;
    spec *= att; //specular
}


void ComputeSpotLight(Material mat, SpotLight L, float3 pos, float3 normal, float3 toEye,
	out float4 ambient, out float4 diffuse, out float4 spec)
{
	// 初始化输出
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// // 从表面到光源的向量
    float3 lightVec = L.Position - pos;

    // 表面到光源的距离
    float d = length(lightVec);

	// 范围测试
    if (d > L.Range)
        return;

	// 标准化光向量
    lightVec /= d;

	// 计算环境光部分
    ambient = mat.Ambient * L.LightIntensity;


    // 计算漫反射光和镜面反射光部分
    float diffuseFactor = dot(lightVec, normal);

	// 展开以避免动态分支
	[flatten]
    if (diffuseFactor > 0.0f)
    {
        //Blinn--Phong模型
        float3 h = normalize(lightVec + toEye);
        float3 halfdir = normalize(lightVec + normal);
        float specFactor = pow(max(saturate(dot(halfdir, normal)), 0.0f), mat.Specular.w);

        diffuse = diffuseFactor * mat.Diffuse * L.LightIntensity;
        spec = specFactor * mat.Specular * L.LightIntensity;
    }

	// 计算汇聚因子和衰弱系数
    float spot = pow(max(dot(-lightVec, L.Direction), 0.0f), L.Spot);
    float att = spot / dot(L.Att, float3(1.0f, d, d * d));

    ambient *= spot;
    diffuse *= att;
    spec *= att;
}