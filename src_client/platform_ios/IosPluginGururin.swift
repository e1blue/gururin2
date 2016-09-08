import Foundation
import Social

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// プラグインクラス
class IosPluginGururin: NSObject{
	// ----------------------------------------------------------------

	// twitter投稿
	static internal func platformTwitter(post: String){
		if(SLComposeViewController.isAvailableForServiceType(SLServiceTypeTwitter)){
			IosPluginUtil.nativePluginUtilLoadingIncrement();
			// twitter投稿開始
			let controller = SLComposeViewController(forServiceType: SLServiceTypeTwitter);
			controller.setInitialText(post);
			controller.completionHandler = {(result: SLComposeViewControllerResult) -> () in
				// 投稿終了
				dispatch_async(dispatch_get_main_queue(), {
					IosPluginUtil.nativePluginUtilLoadingDecrement();
				});
			};
			// 最前面のUIViewControllerを取得
			var viewController = UIApplication.sharedApplication().keyWindow?.rootViewController;
			while((viewController!.presentedViewController) != nil){viewController = viewController!.presentedViewController;}
			// twitterダイアログ表示
			viewController!.presentViewController(controller, animated: true, completion: nil);
		}
	}

	// ----------------------------------------------------------------
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

