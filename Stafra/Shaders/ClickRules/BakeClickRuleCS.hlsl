//Writes non-zero element coordinates from gClickRuleTex into gClickRuleCoordBuf

Texture2D<uint>              gClickRuleTex:      register(t0);
AppendStructuredBuffer<int2> gClickRuleCoordBuf: register(u0);

[numthreads(16, 16, 1)]
void main(uint3 DTid: SV_DispatchThreadID)
{
	uint val = gClickRuleTex[DTid.xy];

	if(val > 0) //Only write non-zero entries
	{
		uint clickRuleWidth  = 0;
		uint clickRuleHeight = 0;
		gClickRuleTex.GetDimensions(clickRuleWidth, clickRuleHeight);

		int2 clickOffset = (int2)(DTid.xy) - int2((clickRuleWidth - 1) / 2, (clickRuleHeight - 1) / 2);
		gClickRuleCoordBuf.Append(int2(clickOffset.x, -clickOffset.y));
	}
}