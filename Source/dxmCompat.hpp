
#ifdef DXMCOMPAT

std::wostream &operator<<(std::wostream &os, const ::DirectX::XMFLOAT2 float2)
{
    return os << float2.x << L' ' << float2.y;
}

std::wostream &operator<<(std::wostream &os, const ::DirectX::XMFLOAT3 float3)
{
    return os << float3.x << L' ' << float3.y << L' ' << float3.z;
}

std::wostream &operator<<(std::wostream &os, const ::DirectX::XMMATRIX mat)
{

    return os << mat.r[0].m128_f32[0] << L' ' << mat.r[0].m128_f32[1] << L' ' << mat.r[0].m128_f32[2] << L' ' << mat.r[0].m128_f32[3] << '\n'
              << mat.r[1].m128_f32[0] << L' ' << mat.r[1].m128_f32[1] << L' ' << mat.r[1].m128_f32[2] << L' ' << mat.r[0].m128_f32[3] << '\n'
              << mat.r[2].m128_f32[0] << L' ' << mat.r[2].m128_f32[1] << L' ' << mat.r[2].m128_f32[2] << L' ' << mat.r[0].m128_f32[3] << '\n'
              << mat.r[3].m128_f32[0] << L' ' << mat.r[3].m128_f32[1] << L' ' << mat.r[3].m128_f32[2] << L' ' << mat.r[0].m128_f32[3] << '\n';
}

std::wostream &operator<<(std::wostream &os, const ::DirectX::XMFLOAT4X4 mat)
{

    return os << mat._11 << L' ' << mat._12 << L' ' << mat._13 << L' ' << mat._14 << '\n'
              << mat._21 << L' ' << mat._22 << L' ' << mat._23 << L' ' << mat._24 << '\n'
              << mat._31 << L' ' << mat._32 << L' ' << mat._33 << L' ' << mat._34 << '\n'
              << mat._41 << L' ' << mat._42 << L' ' << mat._43 << L' ' << mat._44 << '\n';
}
#endif