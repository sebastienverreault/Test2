#include <profile.c>

#define YYYYMMDDHMS	"%Y%m%d %H:%M:%S"

#define UNDER_ALGO		"UNDERLYING"
#define HEDGE_ALGO		"HEDGE"

#define SPOT_SYMBOL0		"BTCUSD"			// original zorro btc data, apparently the same as coinbase
#define INDEX_SYMBOL		"BTC-USD"		// okex underlying USD index data, 24h hitory avail
#define EXCH_SYMBOL2		"USDT-USD"		// okex underlying USDT index data, 24h hitory avail
#define SPOT_SYMBOL		"BTC-USDT"		// okex spot, full hitory avail
#define SWAP_SYMBOL		"BTC-USD-SWAP"	// okex perp swap, full hitory avail


function tradeUnderlying()
{
	if(Asset == SPOT_SYMBOL)
	{
		// Load the underlying
		asset(SPOT_SYMBOL);
		static int btcFirstTime = 1;
		if(btcFirstTime) {
			btcFirstTime = 0;

			// printf("\nPIP: %.8f",PIP);
			// printf("\nPIPCost: %.10f",PIPCost);
			// printf("\nLotAmount: %.5f",LotAmount);

			// Get Current swap price and set varying Pipcost accordingly
			// required due to swap being an inverse futures
			var price = price();
			if(price > 0.) PIPCost = PIP*LotAmount/price;

			// Take position if needed
			var numberBtc = 100 / price;

			// BTC are "traded" exact
			//printf("\nSpread: %.8f",Spread);
			//printf("\nSlippage: %.8f",Slippage);
			// Spread = 0;
			Slippage = 0;

			// printf("\nPIP: %.11f",PIP);
			// printf("\nPIPCost: %.10f",PIPCost);
			// printf("\nLotAmount: %.5f",LotAmount);
			// printf("\nBtcPrice: %.5f",price);
			// printf("\nnumberBtc: %.8f", numberBtc);

			enterLong(numberBtc);
			printf("\nAsset: %s in Algo: %s, Long BTC: %.8f@%.5f", Asset, Algo, numberBtc, price);
		}
	}
	//else {
	//	printf("\nAsset: %s in Algo: %s", Asset, Algo);
	//}
}

function tradeHedge()
{
	if(Asset == SWAP_SYMBOL)
	{
		// Load the swap
		asset(SWAP_SYMBOL);

		// Get Current swap price and set varying Pipcost accordingly
		// required due to swap being an inverse futures
		var price = price();
		if(price > 0.) PIPCost = PIP*LotAmount/price;

		// Take position if needed
		var numberContract = 1;
		static int firstTime = 1;
		if(firstTime) {
			firstTime = 0;

			// BTC are "traded" exact
			//printf("\nSpread: %.8f",Spread);
			//printf("\nSlippage: %.8f",Slippage);
			// Spread = 3.5;
			Slippage = 5;

			enterShort(numberContract);
			printf("\nAsset: %s in Algo: %s, Short PERP: %.5f@%.5f", Asset, Algo, numberContract, price);
			//enterLong(numberContract);
		}
	}
	//else {
	//	printf("\nAsset: %s in Algo: %s", Asset, Algo);
	//}
}

void myPlotProfit()
{
	static var lastBar = 0;
	static var Profit = 0;
	static var PeriodProfit = 0;

	if(Bar != lastBar || Bar == StartBar){
		lastBar = Bar;
		if(is(INITRUN)) Profit = Equity;
		PeriodProfit = Equity - Profit;
		Profit = Equity;
		plot("Win",max(0,PeriodProfit),NEW|BARS,ColorEquity);
		plot("Loss",min(0,PeriodProfit),BARS,ColorDD);
	}
}

void run()
{
	// Log the opening and closing of trades
	Verbose = 8;

	Hedge = 0;

	// No need for past data currently
	LookBack = 0;

	// Execute every X minutes (when BarPeriod = X)
	// Let's say 5min
	BarPeriod = 5;
	// BarPeriod = 1440;

	// Simulation start and stop date
	StartDate = 20210103;
	EndDate = 20210604;

	// No weekends
	resf(BarMode,BR_WEEKEND);
	setf(BarMode,BR_FLAT);
	
	// Record live prices
	priceRecord();	
	

	// Perform or not MC analysis
	MonteCarlo = 0;

	// Load the assset list
	assetList("assetsOkex");



	while(asset(loop(SPOT_SYMBOL,SWAP_SYMBOL)))
	while(algo(loop(UNDER_ALGO,HEDGE_ALGO)))
	{
		// Get capital each bar to simulate deposits and withdrawals
		//printf("\nDate => %s",strdate(YYYYMMDDHMS,0));
		//getCapitalAtTime(strdate(YYYYMMDDHMS,0));
		Capital = 150;

		slider(1,0,0,3000,"Liability","Liability in USD");
		slider(2,0,0,10000,"Capital","Capital posted as margin");
		// slider(3,10000,0,20000,"Profit","Profit so far");

		if(Algo == UNDER_ALGO){
			tradeUnderlying();
		}
		else if(Algo == HEDGE_ALGO) {
			tradeHedge();
		}
		// Plot the Liability
		// Plot the Capital
		myPlotProfit();
	}

	// Save a log file
	// Auto plot a graph
	set(LOGFILE+PLOTNOW);
}
