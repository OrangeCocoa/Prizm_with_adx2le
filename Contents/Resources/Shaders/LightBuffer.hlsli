// LightBuffer計算

// 平行光源
float4 CalcDirectionalLight( float3 objPos, float3 objNormal, float3 litDir, float4 litColor, float litPower )
{
	float3	eyeVec = -normalize( objPos );

	// ライト計算
	float	nl = saturate( dot( objNormal, litDir ) );
	float3	halfVec = normalize( eyeVec + litDir );
	float	nh = saturate( dot( objNormal, halfVec ) );
	float	power = litPower;
	float	spec = 0.0f;
	if( power > 0.0f ){ spec = pow( nh, power ) * nl; }

	// 出力
	return float4( litColor.rgb * nl, spec );
}
float4 CalcDirectionalLightBRDF( float3 objPos, float3 objNormal, float3 litDir, float4 litColor, float litPower, float gloss, float f0 )
{
	float3	eyeVec = -normalize( objPos );

	// ライト計算
	float	nl = saturate( dot( objNormal, litDir ) );
	float3	halfVec = normalize( eyeVec + litDir );
	float	nh = saturate( dot( objNormal, halfVec ) );
	float	power = litPower;
	float	spec = 0.0f;
	if( power > 0.0f )
	{
		float	nv = dot( objNormal, eyeVec );
		float	sn = pow( 2.0, 13.0 * gloss ) * power;
		float	D = (sn + 2.0) * pow(nh, sn) / (2.0 * 3.1415926);
		float	F = f0 + (1.0 - f0) * pow( (1.0 - nv), 5.0 );
		float	dv = dot( eyeVec, (eyeVec + litDir) );
		float	V = 4.0 / (dv * dv);
		float	nl_nv = nl * nv;
		float	G = V * nl_nv;
		spec = F * G * D / (4.0 * nl_nv);
	}

	// 出力
	return float4( litColor.rgb * nl, spec );
}

// 点光源
float4 CalcPointLight( float3 objPos, float3 objNormal, float3 litPos, float4 litColor, float4 litAttn )
{
	float3	eyeVec = -normalize( objPos );

	// 減衰計算
	float3	litDir = litPos.xyz - objPos;
	float	litLen = length( litDir );
	float	litRate = litLen / litAttn.x;
	float	attn = max(1.0 - litRate * litRate, 0.0);
	if( attn == 0.0 )
	{
		return float4(0, 0, 0, 0);
	}
	litDir = normalize( litDir );

	// ライト計算
	float	nl = saturate( dot( objNormal, litDir ) );
	float3	halfVec = normalize( eyeVec + litDir );
	float	nh = saturate( dot( objNormal, halfVec ) );
	float	power = litAttn.w;
	float	spec = 0.0;
	if( power > 0.0 ){ spec = pow( nh, power ) * nl * attn; }

	// 出力
	return float4( litColor.rgb * nl * attn, spec );
}
float4 CalcPointLightBRDF( float3 objPos, float3 objNormal, float3 litPos, float4 litColor, float4 litAttn, float gloss, float f0 )
{
	float3	eyeVec = -normalize( objPos );

	// 減衰計算
	float3	litDir = litPos.xyz - objPos;
	float	litLen = length( litDir );
	float	litRate = litLen / litAttn.x;
	float	attn = max(1.0 - litRate * litRate, 0.0);
	if( attn == 0.0 )
	{
		return float4(0, 0, 0, 0);
	}
	litDir = normalize( litDir );

	// ライト計算
	float	nl = saturate( dot( objNormal, litDir ) );
	float3	halfVec = normalize( eyeVec + litDir );
	float	nh = saturate( dot( objNormal, halfVec ) );
	float	power = litAttn.w;
	float	spec = 0.0;
	if( power > 0.0 )
	{
		float	nv = dot( objNormal, eyeVec );
		float	sn = pow( 2.0, 13.0 * gloss ) * power;
		float	D = (sn + 2.0) * pow(nh, sn) / (2.0 * 3.1415926);
		float	F = f0 + (1.0 - f0) * pow( (1.0 - nv), 5.0 );
		float	dv = dot( eyeVec, (eyeVec + litDir) );
		float	V = 4.0 / (dv * dv);
		float	nl_nv = nl * nv;
		float	G = V * nl_nv;
		spec = max(F * G * D / (4.0 * nl_nv), 0.0);
	}

	// 出力
	return float4( litColor.rgb * nl * attn, spec * attn * attn );
}

