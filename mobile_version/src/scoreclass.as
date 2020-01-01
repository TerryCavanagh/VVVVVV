package {
	import com.milkmangames.nativeextensions.ios.*; 
	import com.milkmangames.nativeextensions.ios.events.*;
	import flash.display.Stage;
	
	public class scoreclass {
		public function init(mystage:Stage):void {
			if (!GameCenter.isSupported()){
				trace("GameCenter is not supported on this platform.");
			}
			
			trace("Loading game center:");
			gameCenter = GameCenter.create(mystage);
			if(!GameCenter.gameCenter.isGameCenterAvailable()){
				trace("this device doesn't have gamecenter.");
			}
			
			if (GameCenter.gameCenter.areBannersAvailable()){
				GameCenter.gameCenter.showAchievementBanners(true);
			}
			
			gameCenter.addEventListener(GameCenterEvent.AUTH_SUCCEEDED,onAuthSucceeded);
			gameCenter.addEventListener(GameCenterErrorEvent.AUTH_FAILED,onAuthFailed);
			gameCenter.addEventListener(GameCenterEvent.ACHIEVEMENTS_VIEW_OPENED,onViewOpened);
			gameCenter.addEventListener(GameCenterEvent.ACHIEVEMENTS_VIEW_CLOSED,onViewClosed);
			gameCenter.addEventListener(GameCenterEvent.LEADERBOARD_VIEW_OPENED,onViewOpened);
			gameCenter.addEventListener(GameCenterEvent.LEADERBOARD_VIEW_CLOSED,onViewClosed);
			gameCenter.addEventListener(GameCenterEvent.ACHIEVEMENT_REPORT_SUCCEEDED,onAchievementReported);
			gameCenter.addEventListener(GameCenterEvent.ACHIEVEMENT_RESET_SUCCEEDED,onAchievementReset);
			gameCenter.addEventListener(GameCenterEvent.SCORE_REPORT_SUCCEEDED,onScoreReported);
			gameCenter.addEventListener(GameCenterErrorEvent.SCORE_REPORT_FAILED,onScoreFailed);
			gameCenter.addEventListener(GameCenterErrorEvent.ACHIEVEMENT_REPORT_FAILED,onAchievementFailed);
			gameCenter.addEventListener(GameCenterErrorEvent.ACHIEVEMENT_RESET_FAILED,onResetFailed);		
			GameCenter.gameCenter.authenticateLocalUser();
		}
		
			
	  CONFIG::iphonemode {
			/** Check Authentication */
			private function checkAuthentication():Boolean{
				if (!GameCenter.gameCenter.isUserAuthenticated()){
					trace("not logged in!");
					return false;
				}
				return true;
			}
			
			/** Reset Achievements */
			public function resetAchievements():void{
				if (!checkAuthentication()) return;
				
				GameCenter.gameCenter.resetAchievements();
			}
			
			public function reportScore(t:int):void{
				// we make sure you're logged in before bothering to report the score.
				// later iOS versions may take care of waiting/resubmitting for you, but earlier ones won't.
				if (!checkAuthentication()) return;
				t = t * 2; //Score is in 30 frame increments
				GameCenter.gameCenter.reportScoreForCategory(t, "grp.supgravleaderboard");
			}
			
			public function opengamecenter():void {
				if (!checkAuthentication()) return;
				GameCenter.gameCenter.showLeaderboardForCategory("grp.supgravleaderboard");
			}
			
			/** Show Achievements */
			public function showAchievements():void{
				if (!checkAuthentication()) return;
				
				//trace("showing achievements...");
				try{
					GameCenter.gameCenter.showAchievements();
				}catch (e:Error){
					//trace("ERR showachievements:"+e.message+"/"+e.name+"/"+e.errorID);
				}
			}
			
			public var vvvvvvgamecomplete:int = 0;
			public var vvvvvvgamecompleteflip:int = 1;
			public var vvvvvvcomplete500:int = 2;
			public var vvvvvvcomplete250:int = 3;
			public var vvvvvvcomplete100:int = 4;
			public var vvvvvvcomplete50:int = 5;
			public var vvvvvvtimetrial_station1:int = 6;
			public var vvvvvvtimetrial_lab:int = 7;
			public var vvvvvvtimetrial_tower:int = 8;
			public var vvvvvvtimetrial_station2:int = 9;
			public var vvvvvvtimetrial_warp:int = 10;
			public var vvvvvvtimetrial_final:int = 11;
			public var vvvvvvsupgrav5:int = 12;
			public var vvvvvvsupgrav10:int = 13;
			public var vvvvvvsupgrav15:int = 14;
			public var vvvvvvsupgrav20:int = 15;
			public var vvvvvvsupgrav30:int = 16
			public var vvvvvvsupgrav60:int = 17;
			public var vvvvvvmaster:int = 18;
			
			public function reportAchievement(t:int):void{
				if (!checkAuthentication()) return;
				
				// the '1.0' is a float (Number) value from 0.0-100.0 the percent completion of the achievement.
				switch(t) {
					case 0: GameCenter.gameCenter.reportAchievement("grp.vvvvvvgamecomplete", 100.0); break;
					case 1: GameCenter.gameCenter.reportAchievement("grp.vvvvvvgamecompleteflip", 100.0); break;
					case 2: GameCenter.gameCenter.reportAchievement("grp.vvvvvvcomplete500", 100.0); break;
					case 3: GameCenter.gameCenter.reportAchievement("grp.vvvvvvcomplete250", 100.0); break;
					case 4: GameCenter.gameCenter.reportAchievement("grp.vvvvvvcomplete100", 100.0); break;
					case 5: GameCenter.gameCenter.reportAchievement("grp.vvvvvvcomplete50", 100.0); break;
					case 6: GameCenter.gameCenter.reportAchievement("grp.vvvvvvtimetrial_station1", 100.0); break;
					case 7: GameCenter.gameCenter.reportAchievement("grp.vvvvvvtimetrial_lab", 100.0); break;
					case 8: GameCenter.gameCenter.reportAchievement("grp.vvvvvvtimetrial_tower", 100.0); break;
					case 9: GameCenter.gameCenter.reportAchievement("grp.vvvvvvtimetrial_station2", 100.0); break;
					case 10: GameCenter.gameCenter.reportAchievement("grp.vvvvvvtimetrial_warp", 100.0); break;
					case 11: GameCenter.gameCenter.reportAchievement("grp.vvvvvvtimetrial_final", 100.0); break;
					case 12: GameCenter.gameCenter.reportAchievement("grp.vvvvvvsupgrav5", 100.0); break;
					case 13: GameCenter.gameCenter.reportAchievement("grp.vvvvvvsupgrav10", 100.0); break;
					case 14: GameCenter.gameCenter.reportAchievement("grp.vvvvvvsupgrav15", 100.0); break;
					case 15: GameCenter.gameCenter.reportAchievement("grp.vvvvvvsupgrav20", 100.0); break;
					case 16: GameCenter.gameCenter.reportAchievement("grp.vvvvvvsupgrav30", 100.0); break;
					case 17: GameCenter.gameCenter.reportAchievement("grp.vvvvvvsupgrav60", 100.0); break;
					case 18: GameCenter.gameCenter.reportAchievement("grp.vvvvvvmaster", 100.0); break;
				}
			}
			
			//
			// Events
			//
			private function onAuthSucceeded(e:GameCenterEvent):void
			{
				trace("Auth succeeded!");
				//showFullUI();
				trace("auth player:" + GameCenter.gameCenter.getPlayerAlias() + "=" + GameCenter.gameCenter.getPlayerID() + ",underage?" + GameCenter.gameCenter.isPlayerUnderage());
				gamecenteron = true;
			}
			
			private function onAuthFailed(e:GameCenterErrorEvent):void
			{
				trace("Auth failed:"+e.message);
				//showAuthUI();
			}
			
			private function onViewOpened(e:GameCenterEvent):void
			{
				trace("gamecenter view opened.");
			}
			private function onViewClosed(e:GameCenterEvent):void
			{
				trace("gamecenter view closed.");
			}
			private function onAchievementReported(e:GameCenterEvent):void
			{
				trace("achievement report success:"+e.achievementID);
			}
			private function onAchievementFailed(e:GameCenterErrorEvent):void
			{
				trace("achievement report failed:msg="+e.message+",cd="+e.errorID+",ach="+e.achievementID);
			}
			private function onScoreReported(e:GameCenterEvent):void
			{
				trace("score report success:"+e.score+"/"+e.category);
			}
			private function onScoreFailed(e:GameCenterErrorEvent):void
			{
				trace("score report failed:msg="+e.message+",cd="+e.errorID+",scr="+e.score+",cat="+e.category);
			}
			private function onAchievementReset(e:GameCenterEvent):void
			{
				trace("achievements reset.");
				//this.score=0;
				//saveScore();
				//this.txtScore.text="Score: "+score;
			}
			private function onResetFailed(e:GameCenterErrorEvent):void
			{
				trace("failed to reset:"+e.message);
			}	
		}
		
		CONFIG::iphonemode { 
		  public var gameCenter:GameCenter;
		  public var gamecenteron:Boolean = true;
		}
	}
}
