// =====================================
// Inputs / Outputs
// =====================================

struct VSOutput
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
};

// =====================================
// Algorithm
// =====================================

VSOutput PostProcessVS(uint vertexId : SV_VertexID)
{
    VSOutput output;

    // Assign the correct position for each 3 vertices to create a big triangle
    // Vertex 1 : (-1, +1)
    // Vertex 2 : (-1, -3)
    // Vertex 3 : (+3, +1)
    float2 vertexPosition = float2
	(
        (vertexId == 2) ? 3.0 : -1.0,
        (vertexId == 1) ? -3.0 : 1.0
    );

    output.pos = float4(vertexPosition, 0.0, 1.0);

    // Convert clip space coordinates to UV coordinates
    output.uv = float2
	(
        vertexPosition.x * 0.5 + 0.5,
        -vertexPosition.y * 0.5 + 0.5
    );

    return output;
}


