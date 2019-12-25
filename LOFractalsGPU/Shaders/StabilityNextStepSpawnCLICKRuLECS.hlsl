cbuffer cbSpawnParams
{
	uint gSpawnPeriod;
};

Texture2D<uint> gPrevBoard:     register(t0);
Texture2D<uint> gPrevStability: register(t1);
Texture2D<uint> gClickRule:     register(t2);

RWTexture2D<uint> gNextBoard:     register(u0);
RWTexture2D<uint> gNextStability: register(u1);

groupshared uint clickRuleDataCache[1024];

[numthreads(32, 32, 1)]
void main(uint3 DTid: SV_DispatchThreadID, uint3 GTid: SV_GroupThreadID, uint GroupIndex: SV_GroupIndex)
{
	clickRuleDataCache[GroupIndex] = gClickRule[GTid.xy];
	GroupMemoryBarrierWithGroupSync();

	uint sum = 0;

	for(uint i = 0; i < 1024; i++)
	{
		int x = i % 32 - 15;
		int y = i / 32 - 15;

		uint cellState = gPrevBoard[int2(DTid.x - x, DTid.y - y)];
		sum += clickRuleDataCache[i] * cellState;
	}

	//Using reflectivity and symmetry
	uint thisCellState = gPrevBoard[int2(DTid.x, DTid.y)];
	uint nextCellState = sum % 2;

	uint prevStability = gPrevStability[DTid.xy];
	uint nextStability = prevStability;
	
	[flatten]
	if(thisCellState == nextCellState)
	{
		[flatten]
		if(prevStability != 1)
		{
			nextStability = (prevStability + 1) % (2 + gSpawnPeriod);
		}
	}
	else
	{
		nextStability = 2;
	}

	gNextBoard[DTid.xy]     = nextCellState;
	gNextStability[DTid.xy] = nextStability;
}