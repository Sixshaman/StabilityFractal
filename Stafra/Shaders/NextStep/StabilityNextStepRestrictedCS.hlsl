Texture2D<uint> gPrevBoard:     register(t0);
Texture2D<uint> gPrevStability: register(t1);
Texture2D<uint> gRestriction:   register(t2);

RWTexture2D<uint> gNextBoard:     register(u0);
RWTexture2D<uint> gNextStability: register(u1);

[numthreads(32, 32, 1)]
void main(uint3 DTid: SV_DispatchThreadID)
{
	//Using reflectivity and symmetry
	int2 thisCellPos   = int2(DTid.x,     DTid.y    );
	int2 leftCellPos   = int2(DTid.x - 1, DTid.y    );
	int2 rightCellPos  = int2(DTid.x + 1, DTid.y    );
	int2 topCellPos    = int2(DTid.x,     DTid.y - 1);
	int2 bottomCellPos = int2(DTid.x,     DTid.y + 1);

	uint thisCellState    = gPrevBoard[thisCellPos  ] * gRestriction[thisCellPos  ];
	uint leftCellState    = gPrevBoard[leftCellPos  ] * gRestriction[leftCellPos  ];
	uint rightCellState   = gPrevBoard[rightCellPos ] * gRestriction[rightCellPos ];
	uint topCellState     = gPrevBoard[topCellPos   ] * gRestriction[topCellPos   ];
	uint bottomCellState  = gPrevBoard[bottomCellPos] * gRestriction[bottomCellPos];

	uint nextCellState = (thisCellState + leftCellState + rightCellState + topCellState + bottomCellState) % 2;

	uint prevStability = gPrevStability[DTid.xy];
	uint nextStability = (prevStability && (thisCellState == nextCellState));

	gNextBoard[DTid.xy]     = nextCellState;
	gNextStability[DTid.xy] = nextStability * gRestriction[DTid.xy];
}