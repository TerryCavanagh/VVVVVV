package {
	import flash.geom.*;
  import flash.events.*;
  import flash.net.*;
	
	public class LevelMetaData{
		public function LevelMetaData():void {
			clear();
    }
		
		public function clear():void {
		  title = "";
	    creator = "";
	    Desc1 = "";
	    Desc2 = "";
	    Desc3 = "";
	    website = "";
  	  filename = "";
			file_num = 0;
	    modifier = "";
	    timeCreated = "";
	    timeModified = "";
		  version = 0;
		}
		
		
		public var title:String;
	  public var creator:String;
	  public var Desc1:String;
	  public var Desc2:String;
	  public var Desc3:String;
	  public var website:String;
  	public var filename:String;
  	public var file_num:int;
	  public var modifier:String;
	  public var timeCreated:String;
	  public var timeModified:String;
		
		public var version:int;
	}
};
