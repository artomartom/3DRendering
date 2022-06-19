#include "Func.hlsl"


cbuffer ModelBuffer : register(b0)
{
    matrix model;
};  

//  t - time since application start 
//  t milisec from start (1.f = 1 sec)
//  deltaT - time since last frame  
cbuffer FrameBuffer : register(b1)
{ 
    matrix view;
    float2 frameTime; //   (  t  , (t % (1000 * 60 * 10))  , deltaT,deltaT)
}; 

cbuffer  ProjectionBuffer : register(b2)
{
    matrix projection;
};  
static float3 cube[8] = 
{   
    {-.5f, +.5f,+.5f},
    {+.5f, +.5f,+.5f},
    {+.5f, -.5f,+.5f},
    {-.5f, -.5f,+.5f},
    {-.5f, +.5f,-.5f},
    {+.5f, +.5f,-.5f},
    {+.5f, -.5f,-.5f},
    {-.5f, -.5f,-.5f},
};
static uint  indices[] =
{
    3,1,0,
    2,1,3, 

    0,5,4,
    1,5,0,
    
    3,4,7,
    0,4,3,
    
    1,6,5,
    2,6,1,
    
    2,7,6,
    3,7,2,
    
    6,4,5,
    7,4,6,
};

float4 VSMain(uint VertID:SV_VertexID, uint InstID:SV_InstanceID) : SV_Position
{

    uint index = indices[VertID];
    
    float4 pos = float4(cube[index] *.8f,1.0) ;
    pos = mul( mul( mul( pos, model ), view ), projection );
    return  pos;  
};

float4 PSMain(float4 pos :SV_Position ) : SV_Target
{
    return    float4(  pos.xy,.0, 1.0) ;  
};

 



  