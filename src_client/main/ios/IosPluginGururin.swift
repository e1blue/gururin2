import Foundation
import Social

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// プラグインクラス
class IosPluginGururin: NSObject{
	// ----------------------------------------------------------------

	// twitter投稿
	static internal func platformTwitter(_ post: String){
		if(SLComposeViewController.isAvailable(forServiceType: SLServiceTypeTwitter)){
			IosPluginUtil.nativePluginUtilLoadingIncrement();
			// twitter投稿開始
			let controller = SLComposeViewController(forServiceType: SLServiceTypeTwitter);
			controller?.setInitialText(post);
			controller?.completionHandler = {(result: SLComposeViewControllerResult) -> () in
				// 投稿終了
				DispatchQueue.main.async(execute: {
					IosPluginUtil.nativePluginUtilLoadingDecrement();
				});
			};
			// 最前面のUIViewControllerを取得
			var viewController = UIApplication.shared.keyWindow?.rootViewController;
			while((viewController!.presentedViewController) != nil){viewController = viewController!.presentedViewController;}
			// twitterダイアログ表示
			viewController!.present(controller!, animated: true, completion: nil);
		}
	}

	// ----------------------------------------------------------------
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

