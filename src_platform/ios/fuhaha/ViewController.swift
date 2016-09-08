import UIKit
import GoogleMobileAds

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

class ViewController: UIViewController{
	private let statusView: UIView = UIView();
	private let glViewController: FuhahaGLViewController = FuhahaGLViewController();
	private let bannerView: GADBannerView = GADBannerView();

	override func viewDidLoad(){
		super.viewDidLoad();
		// Do any additional setup after loading the view, typically from a nib.

		// ステータスバー
		self.statusView.backgroundColor = UIColor.whiteColor();
		self.view.addSubview(self.statusView);

		// ゲーム本体
		self.view.addSubview(self.glViewController.view);

		// 広告
		self.bannerView.adUnitID = IosPluginSecret.iosPluginSecretGet("goodfox", id: 10010001);
		self.bannerView.rootViewController = self;
		let request: GADRequest = GADRequest();
		request.testDevices = [kGADSimulatorID, IosPluginSecret.iosPluginSecretGet("goodfox", id: 10020001)];
		self.bannerView.loadRequest(request);
		self.view.addSubview(self.bannerView);
	}

	override func viewDidLayoutSubviews(){
		super.viewDidLayoutSubviews();
		let parentWidth: CGFloat = self.view.frame.size.width;
		let parentHeight: CGFloat = self.view.frame.size.height;
		let statusHeight: CGFloat = UIApplication.sharedApplication().statusBarFrame.height;
		let bannerWidth: CGFloat = kGADAdSizeBanner.size.width;
		let bannerHeight: CGFloat = kGADAdSizeBanner.size.height;

		var stRect: CGRect = self.statusView.frame;
		stRect.size.width = parentWidth;
		stRect.size.height = statusHeight;
		self.statusView.frame = stRect;

		var glRect: CGRect = self.glViewController.view.frame;
		glRect.origin.y = statusHeight;
		glRect.size.width = parentWidth;
		glRect.size.height = parentHeight - statusHeight - bannerHeight;
		self.glViewController.view.frame = glRect;

		var adRect: CGRect = self.bannerView.frame;
		adRect.origin.x = (parentWidth - bannerWidth) * 0.5;
		adRect.origin.y = parentHeight - bannerHeight;
		adRect.size.width = bannerWidth;
		adRect.size.height = bannerHeight;
		self.bannerView.frame = adRect;
	}

	override func didReceiveMemoryWarning(){
		super.didReceiveMemoryWarning();
		// Dispose of any resources that can be recreated.
	}
}
// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

