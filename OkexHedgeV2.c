#include <profile.c>

#define YYYYMMDDHMS	"%Y%m%d %H:%M:%S"

#define UNDER_ALGO		"UNDERLYING"
#define HEDGE_ALGO		"HEDGE"

function tradeUnderlying()
{
	if(Asset == "BTCUSD")
	{
		// Load the underlying
		asset("BTCUSD");
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
	if(Asset == "BTC-PERPETUAL")
	{
		// Load the swap
		asset("BTC-PERPETUAL");

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

	// Perform or not MC analysis
	MonteCarlo = 0;

	// Load the assset list
	assetList("assetsOkex");



	while(asset(loop("BTCUSD","BTC-PERPETUAL")))
	while(algo(loop(UNDER_ALGO,HEDGE_ALGO)))
	{
		// Get capital each bar to simulate deposits and withdrawals
		//printf("\nDate => %s",strdate(YYYYMMDDHMS,0));
		//getCapitalAtTime(strdate(YYYYMMDDHMS,0));
		Capital = 150;

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
