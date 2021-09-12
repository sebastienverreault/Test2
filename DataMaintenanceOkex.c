#define SPOT_SYMBOL0	"BTCUSD"			// original zorro btc data
#define INDEX_SYMBOL	"BTC-USD"		// okex underlying USD index data, 24h hitory avail
#define EXCH_SYMBOL2	"USDT-USD"		// okex underlying USDT index data, 24h hitory avail
#define SPOT_SYMBOL	"BTC-USDT"		// okex spot, full hitory avail
#define SWAP_SYMBOL	"BTC-USD-SWAP"	// okex perp swap, full hitory avail

function run()
{
	StartDate = 20210101;
	EndDate = 20211231;
	LookBack = 0;
	BarPeriod = 1;

	resf(BarMode,BR_WEEKEND);
	setf(BarMode,BR_FLAT);
	
	while(asset(loop(INDEX_SYMBOL, EXCH_SYMBOL2, SPOT_SYMBOL, SWAP_SYMBOL)))
	{
		assetHistory(Asset, 1);
		priceRecord();	
	}
}
