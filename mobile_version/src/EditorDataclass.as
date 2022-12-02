package {
	public class EditorDataclass{
		public function EditorDataclass():void {
			clear();
    }
		
		public function clear():void {
			numedentities = 0;
			title = "";
			creator = "";
			modifier = "";
			timeCreated = "";
			timeModified = "";
		}
		
		public var numedentities:int;
	  public var title:String;
	  public var creator:String;
	  public var modifier:String;
	  public var timeCreated:String;
	  public var timeModified:String;
	}
};