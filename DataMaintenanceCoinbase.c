#define SPOT_SYMBOL0	"BTCUSD"			// original zorro btc data
#define SPOT_SYMBOL	"BTC/USD"		// coinbase spot btc

function run()
{
	StartDate = 20210101;
	EndDate = 20211231;
	LookBack = 0;
	BarPeriod = 1;
	
	resf(BarMode,BR_WEEKEND);
	setf(BarMode,BR_FLAT);

	while(asset(loop(SPOT_SYMBOL)))
	{
		assetHistory(Asset, 1);
		priceRecord();
	}
}
