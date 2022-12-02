package {
	public class EmbeddedAssets {
		/* PNG texture */
    [Embed(source = "../data/vvvvvv_graphics.png")]
    public static const vvvvvv_graphics:Class;
		
		/* XML file */
    [Embed(source   = "../data/vvvvvv_graphics.xml",
           mimeType = "application/octet-stream")]
    public static const vvvvvv_graphics_xml:Class;
	}
}