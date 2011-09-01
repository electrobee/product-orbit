package be.haxer;
import neko.io.File;
import neko.FileSystem;
import neko.Lib;
import neko.Sys;
import haxe.io.Bytes;
import haxe.io.BytesInput;
import haxe.io.BytesOutput;
import haxe.io.Input;
import format.abc.Context;
import format.abc.Data;
import format.swf.Constants;
import format.swf.Data;
import format.swf.Tools;
import format.tools.BitsInput;
import format.tools.Inflate;
/**
* ...
* @author Jan J.Flanders
*/
class ABCSymbol
{
	static var i:Input;
	static var bits:BitsInput;
	static var chars:IntHash<String> = new IntHash();
	static var tagNames:IntHash<String> = new IntHash();
	static var tags:Array<SWFTag> = new Array();// [TSandBox(8)];
	static var idx:Int = 0;
	static var doLog:Bool = false;
	static var logBuf:StringBuf = new StringBuf();
	static var modified:Bool=false;
	
	inline static function log(msg:String) 
		logBuf.add(msg+"\n")
		
	inline static function setupTagNames()	
		for(f in Type.getClassFields(TagId)) 
			tagNames.set(Reflect.field(TagId,f),f)
			
	static function main() 
	{
		var args = Sys.args();
		if(args.length < 2)
			Lib.print("Usage: abcsymbol.exe in.swf out.swf [-log]")
		else
		{
			var fileIn = args[0];
			var fileOut = args[1];
			doLog = args[2] == "-log";
			
			if(doLog) setupTagNames();
			if(FileSystem.exists(fileIn))
			{
				var newSwf = parseAndInject(File.getBytes(fileIn));
				modified?File.write(fileOut,true).write(newSwf):log("\n! There are no linkable symbols or names present in the swf.\nAbc injection was skipped.");
			}
			else
				Lib.print('Error:' + fileIn + ' could not be found.');
		}
		if(doLog)Lib.print(logBuf.toString());
	}
	
	static function parseAndInject(swf):Bytes
	{
		i = new BytesInput(swf);
		i.bigEndian = false;
		var o = new BytesOutput();
		o.bigEndian = false;
		var w = new format.swf.Writer(o);
		w.write({header:readHeader(), tags:readTagList()});
		return o.getBytes();
	}
	
	static function readHeader()
	{
		var compressed = i.readString(3) == "CWS";
		var version = i.readByte();
		var size = i.readUInt30();
		if(compressed) 
		{
			var bytes = Inflate.run(i.readAll());
			if( bytes.length + 8 != size ) 
				throw ('wrong bytes length after decompression');
			i = new BytesInput(bytes);
		}
		bits = new BitsInput(i);
		var r = readRect();
		i.readByte();//ignore first byte for fps
		var header=
		{
			compressed : compressed,
			version : version,
			width : Std.int(r.right/20),
			height : Std.int(r.bottom/20),
			fps : i.readByte(),
			nframes : i.readUInt16(),
		};
		idx++;
		if(doLog)log('0) SWF-Header\tSize:'+size+', compressed:' + header.compressed + ', version:' + header.version +', width:'+ header.width + ', height:' + header.height + ', fps:' + header.fps + ', total frames:' + header.nframes);
		return header;
	}
	
	inline static function readTagList():Array<SWFTag>
	{
		while(true) 
		{
			var t = readTag();
			if( t == null )
				break;
			tags.push(t);
			idx++;
		}
		return tags;
	}
	
	inline static function readTag():SWFTag 
	{
		var h = i.readUInt16();
		var id = h >> 6;
		var len = h & 63;
		if( len == 63 ) 
			len = i.readUInt30();
		return switch( id ) 
		{
			case TagId.DefineFont, TagId.DefineFont2, TagId.DefineFont3 : 
				createTagStoreId(id,len,"flash.text.Font");
			case TagId.DefineBitsLossless, TagId.DefineBitsLossless2, TagId.DefineBits, TagId.DefineBitsJPEG2, TagId.DefineBitsJPEG3 : 
				createTagStoreId(id,len,"flash.display.Bitmap");
			case TagId.DefineSprite : 
				createTagStoreId(id,len,"flash.display.MovieClip");
			case TagId.DefineBinaryData: 
				createTagStoreId(id,len,"flash.utils.ByteArray");
			case TagId.DefineSound: 
				createTagStoreId(id,len,"flash.media.Sound");
			case TagId.DefineButton2: 
				createTagStoreId(id,len,"flash.display.Button");
			case TagId.SymbolClass:
				createSymbolClassTag(id);
			case TagId.End:
				if(doLog)log(idx+") SWF-tag\t0\tEnd");
				return null;
			default: 
				id==0x38/*ExportAssets*/?
					createSymbolClassTag(id)
					:createGeneralTag(id, len);
		}
	}
	
	inline static function createTagStoreId(id:Int, len, baseClass):SWFTag
	{
		var data = i.read(len);
		var inp = new BytesInput(data);
		inp.bigEndian = false;
		var cid = inp.readUInt16();
		chars.set(cid, baseClass);
		if(doLog)log(idx+") SWF-tag\t"+ id + "\t"+ tagNames.get(id)+" (id:" + cid + ", type:" + baseClass.split(".").pop() + ")");
		return TUnknown(id,data);
	}
	
	inline static function createGeneralTag(id:Int, len):SWFTag
	{
		if(doLog)log(idx+") SWF-tag\t" + id + "\t" + tagNames.get(id));
		return TUnknown(id,i.read(len));
	}
	
	inline static function createSymbolClassTag(id:Int):SWFTag
	{
		var symbols = readSymbols();
		if(doLog)log(idx+") SWF-tag\t" + id + "\t" + tagNames.get(id));
		for(s in symbols)
			if(chars.get(s.cid)!=null)//check for tag72 abc tags which do not point to linkable symbols
			{
				tags.push(createABC(s.className, chars.get(s.cid)));
				if(doLog)log("-> Injected abc for asset id:" + s.cid+" -> " +"class " + s.className +" extends " + chars.get(s.cid) + "{}\n"+
					(idx<10?" ":"  " ) + ") SWF-tag\t82\tDoABC\n" +
					(idx<10?" ":"  " ) + ") SWF-tag\t76\tSymbolClass" + " (asset id:" + s.cid + ", class name:" + s.className + ")");
				modified=true;
			}
		return TSymbolClass(symbols);
	}
	
	inline static function readSymbols()
	{
		var symbols:Array<{cid:Int, className:String}> = new Array();
		for ( n in 0...i.readUInt16() )
			symbols.push({cid:i.readUInt16(), className:i.readUntil(0)});
		return symbols;
	}
	
	inline static function readRect() 
	{
		bits.reset();
		var nbits = bits.readBits(5);
		return {
			left : Tools.signExtend(bits.readBits(nbits), nbits),
			right : Tools.signExtend(bits.readBits(nbits), nbits),
			top : Tools.signExtend(bits.readBits(nbits), nbits),
			bottom : Tools.signExtend(bits.readBits(nbits), nbits)
		};
	}
	
	inline static function createABC(className:String, baseClass:String):SWFTag
	{
		var ctx = new Context();
		var c = ctx.beginClass(className, false);
		c.superclass = ctx.type(baseClass);
		switch(baseClass)
		{
			case 'flash.display.MovieClip' : 	
				ctx.addClassSuper("flash.events.EventDispatcher");
				ctx.addClassSuper("flash.display.DisplayObject");
				ctx.addClassSuper("flash.display.InteractiveObject");
				ctx.addClassSuper("flash.display.DisplayObjectContainer");
				ctx.addClassSuper("flash.display.Sprite");
				ctx.addClassSuper("flash.display.MovieClip");

			case 'flash.display.Sprite' : 
				ctx.addClassSuper("flash.events.EventDispatcher");
				ctx.addClassSuper("flash.display.DisplayObject");
				ctx.addClassSuper("flash.display.InteractiveObject");
				ctx.addClassSuper("flash.display.DisplayObjectContainer");
				ctx.addClassSuper("flash.display.Sprite");
				
			case 'flash.display.SimpleButton' : 
				ctx.addClassSuper("flash.events.EventDispatcher");
				ctx.addClassSuper("flash.display.DisplayObject");
				ctx.addClassSuper("flash.display.InteractiveObject");
				ctx.addClassSuper("flash.display.SimpleButton");
			
			case 'flash.display.Bitmap' : 
				ctx.addClassSuper("flash.events.EventDispatcher");
				ctx.addClassSuper("flash.display.DisplayObject");
				ctx.addClassSuper("flash.display.Bitmap");

			case 'flash.media.Sound' : 
				ctx.addClassSuper("flash.events.EventDispatcher");
				ctx.addClassSuper("flash.media.Sound");
				
			case 'flash.display.BitmapData' : 
				ctx.addClassSuper("flash.display.BitmapData");
				
			case 'flash.text.Font' : 
				ctx.addClassSuper("flash.text.Font");
			
			case 'flash.utils.ByteArray' : 
				ctx.addClassSuper("flash.utils.ByteArray");
		}
		var m = baseClass!='flash.display.BitmapData'? ctx.beginMethod(className, [], null, false, false, false, true):ctx.beginMethod(className, [ctx.type('Number'),ctx.type('Number')], null, false, false, false, true);
		m.maxStack = 3;
		c.constructor = m.type;
		ctx.ops( baseClass!='flash.display.BitmapData'? [OThis,OConstructSuper(0),ORetVoid]  :  [OThis,OReg(1),OReg(2),OConstructSuper(2),ORetVoid] );
		ctx.endClass();
		var abcOutput = new BytesOutput();
		format.abc.Writer.write(abcOutput, ctx.getData());
		return TActionScript3(abcOutput.getBytes(), {id : 1, label : className});
	}
}