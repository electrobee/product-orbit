import flash.display.DisplayObject;
import flash.Lib;
import flash.events.Event;
import flash.events.MouseEvent;
import flash.display.MovieClip;
import flash.display.Sprite;

import flash.text.TextField;
import flash.text.TextFormat;
import flash.text.TextFieldAutoSize;
import flash.text.TextLineMetrics;
import flash.display.BlendMode;
import flash.utils.Timer;
import flash.events.TimerEvent;
import flash.text.AntiAliasType;
import flash.ui.Mouse;

import flash.filters.BlurFilter;
import flash.geom.Point;
import flash.geom.Matrix;

import flash.filters.GlowFilter;
import flash.geom.ColorTransform;

import flash.text.Font;
import flash.text.TextField;

class Symbols extends flash.text.Font {}

class ToolTip extends Sprite {
	private var _tip:String;
	private var _obj:DisplayObject;
	private var _tf:TextField;
	private var _format:TextFormat;
	private var _timer:Timer;
	private var _show:Bool;
	
	private static var ROUND:Float = 10;
	private static var HPADDING:Float = 4;
	private static var VPADDING:Float = 2;
	private static var ALPHA_MIN:Float = 0.00;
	private static var ALPHA_INC:Float = 0.05;
	private static var ALPHA_MAX:Float = 0.9;
	
	public function new(obj:DisplayObject, tip:String) {
		super();
		
		_obj = obj;
		_tip = tip;
		_show = false;
		_tf = new TextField();
		_format = new TextFormat();
		_format.size = 12;
		_format.font = "Arial";
		
		_tf.textColor = 0xFFFFFF;
		_tf.antiAliasType = AntiAliasType.ADVANCED;
		_tf.defaultTextFormat = _format;
		_tf.autoSize = TextFieldAutoSize.LEFT;
		_tf.multiline = false;
		_tf.text = _tip;
		_tf.selectable = false;
		_tf.blendMode = BlendMode.LAYER;
		
		_timer = new Timer(10 /* milliseconds */);
		_timer.addEventListener(TimerEvent.TIMER, timerHandler);
		
		addChild(_tf);
		
		this.mouseEnabled = false;
		this.buttonMode = false;
		this.mouseChildren = false;
		
		this.alpha = ALPHA_MIN;
		
		_obj.addEventListener(MouseEvent.MOUSE_OVER, mouseOver);
		_obj.addEventListener(MouseEvent.MOUSE_OUT, mouseOut);
	}
	
	private function timerHandler(event:TimerEvent) {
		if (_show) {
			alpha += ALPHA_INC;
			if (alpha >= ALPHA_MAX) {
				alpha = ALPHA_MAX;
				_timer.stop();
			}
		} else {
			alpha -= ALPHA_INC;
			if (alpha <= ALPHA_MIN) {
				alpha = ALPHA_MIN;
				_timer.stop();
			}
		}
	}
	
	public function draw(_x:Float, _y:Float) {
		_x += 10;
		_y -= height / 2;
		
		_tf.x = _x + HPADDING;
		_tf.y = _y + VPADDING;

		graphics.clear();
		//graphics.lineStyle(1, 0xFFFFFF, 0.7, true);
		graphics.beginFill( 0x000000, 1.0 );
		graphics.drawRoundRect( _x, _y, _tf.width + HPADDING*2, _tf.height + 1 + VPADDING*2, ROUND );
		graphics.endFill();
	}
	
	public function mouseOver( e:MouseEvent ):Void {
		this.parent.setChildIndex(this, this.parent.numChildren - 1); // tool tip to the top
		draw(e.stageX, e.stageY);
		_show = true;
		_timer.start();
	}
	
	public function mouseOut( e:MouseEvent ):Void {
		_show = false;
		_timer.start();
	}
}

class Feature extends Sprite {
	
	private var highlight:Bool;
	private var w:Float;
	private var h:Float;
	private var tip:String;
	private var frame:UInt;
	private var _timer:Timer;
	private var _show:Bool;
	private var _visible:Bool;
	
	private static var ALPHA_MIN:Float = 0.00;
	private static var ALPHA_INC:Float = 0.05;
	private static var ALPHA_MAX:Float = 0.9;
	
	public function new(_frame:UInt, _x:Float, _y:Float, _width:Float, _height:Float, _tip:String) {
		super();
		
		highlight = false;
		
		frame = _frame;
		x = _x;
		y = _y;
		w = _width;
		h = _height;
		tip = _tip;
		_show = false;
		alpha = ALPHA_MIN;
		_visible = frame == 0;
		
		_timer = new Timer(10 /* milliseconds */);
		_timer.addEventListener(TimerEvent.TIMER, timerHandler);
		
		draw();
		
		var toolTip:ToolTip = new ToolTip(this, tip);
		Main.mc.addChild(toolTip);
		
		addEventListener(MouseEvent.MOUSE_OVER, mouseOver);
		addEventListener(MouseEvent.MOUSE_OUT, mouseOut);
		
		Main.mc.stage.addEventListener(MouseEvent.MOUSE_OVER, mouseOverStage);
		Main.mc.stage.addEventListener(MouseEvent.MOUSE_OUT, mouseOutStage);
		Main.mc.stage.addEventListener(Event.MOUSE_LEAVE, mouseOutStage);
	}
	
	private function timerHandler(event:TimerEvent) {
		if (_show) {
			alpha += ALPHA_INC;
			if (alpha >= ALPHA_MAX) {
				alpha = ALPHA_MAX;
				_timer.stop();
			}
		} else {
			alpha -= ALPHA_INC;
			if (alpha <= ALPHA_MIN) {
				alpha = ALPHA_MIN;
				_timer.stop();
			}
		}
	}
	
	public function show() {
		_show = true;
		_timer.start();
	}
	
	public function hide() {
		_show = false;
		_timer.start();
	}
	
	public function draw() {
		graphics.clear();
		graphics.lineStyle(1, 0x000000, 0.7, true);
		graphics.beginFill(0xffffff, highlight ? 0.5 : 0.2);
		graphics.drawRoundRect(0, 0, w, h, 10, 10);
		graphics.endFill();
	}
	
	public function mouseOver(event:Event):Void {
		highlight = true;
		draw();
	}
	
	public function mouseOut(event:Event):Void {
		highlight = false;
		draw();
		event.stopImmediatePropagation();
	}
	
	public function mouseOverStage(event:Event):Void {
		if(_visible) {
			show();
		}
	}
	
	public function mouseOutStage(event:Event):Void {
		if(_visible) {
			hide();
		}
	}
	
	public function updateFrame(_frame:UInt):Void {
		if (_frame == frame) {
			_visible = true;
			mouseEnabled = true;
			show();
		} else {
			_visible = false;
			mouseEnabled = false;
			hide();
		}
	}
}

class Util {
	public static function scaleAroundCenter(objToScale:DisplayObject, center:Point, scaleX:Float, scaleY:Float):Void{
		var transformedVector:Point = new Point((center.x - objToScale.x) * scaleX, (center.y - objToScale.y) * scaleY);
		
		objToScale.x = center.x - (transformedVector.x);
		objToScale.y = center.y - (transformedVector.y);
		objToScale.scaleX = objToScale.scaleX*(scaleX);
		objToScale.scaleY = objToScale.scaleY*(scaleY);
	}
	
	public static function rotateAroundCenter(ob:DisplayObject, center:Point, angleDegrees:Float) {
		var m:Matrix = ob.transform.matrix;
		m.tx -= center.x;
		m.ty -= center.y;
		m.rotate (angleDegrees * (Math.PI / 180));
		m.tx += center.x;
		m.ty += center.y;
		ob.transform.matrix = m;
	}
}

class Preloader {
	public static var mc : MovieClip = flash.Lib.current;
	private var first:MovieClip;
	var txtPercentage:TextField;
	private var percentage:Int;
	private var timer:Timer;
	
	private var orbits:Array<MovieClip>;
	private static var numOrbits:Int = 6;
	
	public function new() {
		percentage = 0;
		
		first = flash.Lib.attach("first");
		first.alpha = 0;
		mc.addChild(first);
		
		// shuffle array of angles
		var a:Array<Float> = new Array<Float>();
		var angle:Float = 0;
		var deltaAngle:Float = 360 / numOrbits;
		for (i in 0...numOrbits) {
			a[i] = angle;
			angle += deltaAngle;
		}
		var i:Int = a.length -1;
		while (i > 0) {
			var j:Int = Math.floor(Math.random() * (i + 1));
			var tmp:Float = a[i];
			a[i] = a[j];
			a[j] = tmp;
			i--;
		}
		
		orbits = new Array<MovieClip>();
		
		var colors:Array<Int> = [0xFF0000, 0x00FF00, 0x3333FF];
		var scales:Array<Float> = [1.0, 0.7, 1.1, 0.9, 1.0, 0.8, 0.9];
		
		var colorIndex:Int = 0;
		var scaleIndex:Int = 0;
		for (i in 0...numOrbits) {
			var orbit:MovieClip = flash.Lib.attach("Orbit");
			orbit.stop();
			orbit.x = orbit.y = -80;
			orbit.x += mc.width / 2;
			orbit.y += mc.height / 2;
			var center:Point = new Point(mc.width / 2, mc.height / 2);
			Util.scaleAroundCenter(orbit, center, 0.5 * scales[scaleIndex], scales[scaleIndex]);
			Util.rotateAroundCenter(orbit, center, a[i]);
			var colorTransform:ColorTransform = mc.transform.colorTransform;
			colorTransform.color = colors[colorIndex];
			orbit.transform.colorTransform = colorTransform;
			orbit.filters = [new GlowFilter(colors[colorIndex], 0.5, 20, 20, 7.0)];
			orbit.blendMode = BlendMode.HARDLIGHT;
			colorIndex = (colorIndex + 1) % colors.length;
			scaleIndex = (scaleIndex + 1) % scales.length;
			mc.addChild(orbit);
			orbits.push(orbit);
		}
		orbits[0].play();
		
		txtPercentage = new TextField();
		txtPercentage.embedFonts = true;
		txtPercentage.defaultTextFormat = new TextFormat("Symbols", 25, 0xEE0000);
		txtPercentage.text = "0";
		txtPercentage.selectable = false;
		txtPercentage.autoSize = TextFieldAutoSize.LEFT;
		mc.addChild(txtPercentage);
		txtPercentage.x = (mc.width - txtPercentage.width) / 2;
		txtPercentage.y = (mc.height - txtPercentage.height) / 2;
		//txtPercentage.filters = [new GlowFilter(0xFFFFFF, 1.0, 40, 40, 5.0)];
		
		// debug
		//timer = new Timer(100 /* milliseconds */);
		//timer.addEventListener(TimerEvent.TIMER, timerHandler);
		//timer.start();
	}
	
	public function setPercentage(newPercentage:Int) {
		if (newPercentage > 100) newPercentage = 100;
		var from:Int = Math.floor(percentage / 100.0 * numOrbits);
		var to:Int = Math.floor(newPercentage / 100.0 * numOrbits);
		if(from != to && newPercentage < 100) {
			for (i in from+1...to+1) {
				orbits[i].play();
			}
		}
		
		percentage = newPercentage;
		txtPercentage.text = "" + percentage + "%";
	}
	
	private function timerHandler(event:TimerEvent) {
		setPercentage(percentage + 1);
	}
	
	public function remove() {
		for (i in 0...numOrbits) {
			orbits[i].stop();
			mc.removeChild(orbits[i]);
			orbits[i] = null;
		}
		mc.removeChild(txtPercentage);
		txtPercentage = null;
		if(first != null && mc.contains(first))
			mc.removeChild(first);
		first = null;
	}
}

class Main 
{
	public static var mc : MovieClip = flash.Lib.current;
	public static var loaded : Bool = false;
	var img:MovieClip;
	var logo:MovieClip;
	var cursor:MovieClip;
	
	private var dragging:Bool;
	private var fromX:Float;
	private var fromY:Float;
	private var fromFrame:Int;
	private var fromTime:Int;
	private var currFrame:Int;

	private var numFrames:Int;
	
	private static var features:Array<Feature>;
	
	private var prevX:Float;
	private var prevTime:Int;
	private var deltaX:Float;
	private var deltaTime:Int;
	private var blurFilter:BlurFilter;
	private var timer:Timer;
	private var speed:Float;
	private var currPos:Float;
	
	private var currScale:Float;
	
	var acceleration:Float;
	static var preloader:Preloader;
	
	private function initFeatures() {
		var xml:Xml = Xml.parse(haxe.Resource.getString("features")).firstElement();
		for (element in xml.elements()) {
			if(element.nodeName == "feature") { 
				features.push(new Feature(Std.parseInt(element.get("frame")), Std.parseFloat(element.get("x")), Std.parseFloat(element.get("y")), Std.parseFloat(element.get("width")), Std.parseFloat(element.get("height")), element.get("tip")));
			} else if (element.nodeName == "watermark") { 
				logo = flash.Lib.attach("logo");
				logo.x = Std.parseFloat(element.get("x"));
				logo.y = Std.parseFloat(element.get("y"));
				logo.mouseEnabled = false;
				logo.mouseChildren = false;
				mc.addChild(logo);
			}
		}
	}

	static function main() {
		//var app:Main = new Main();
		mc.stop();
		//trace("Starting preloader");
		Lib.current.root.addEventListener(Event.ENTER_FRAME, onEnterFrame);
		
		preloader = new Preloader();
	}
	
	static function onEnterFrame(event : Event) {
		if(!loaded) {
			//trace("Downloaded " + Lib.current.root.loaderInfo.bytesLoaded + " of " + Lib.current.root.loaderInfo.bytesTotal + " bytes");
			preloader.setPercentage(Math.round(Lib.current.root.loaderInfo.bytesLoaded * 100 / Lib.current.root.loaderInfo.bytesTotal));
			if (Lib.current.root.loaderInfo.bytesLoaded >= Lib.current.root.loaderInfo.bytesTotal) {
				loaded = true;
				//trace("Preloading finished");
				preloader.remove();
				preloader = null;
				mc.gotoAndStop(2);
			}
		} else {
			Lib.current.root.removeEventListener(Event.ENTER_FRAME, onEnterFrame);
			var app:Main = new Main();
		}
	}
	
	public function new() {
		//trace("totalFrames = " + flash.Lib.current.totalFrames);
		//trace("currentFrame = " + flash.Lib.current.currentFrame);
		img = flash.Lib.attach("img");
		numFrames = img.totalFrames;
		currFrame = 0;
		dragging = false;
		
		//mc.removeChild(first);
		
        mc.addChild(img);
		img.gotoAndStop(1);
		
		//trace("Num Frames: " + numFrames);
		
		features = new Array<Feature>();
		initFeatures();
		
		mc.stage.addEventListener(MouseEvent.MOUSE_MOVE, mouseMove);
		mc.stage.addEventListener(MouseEvent.MOUSE_DOWN, mouseDown);
		mc.stage.addEventListener(MouseEvent.MOUSE_UP, mouseUp);
		mc.stage.addEventListener(MouseEvent.MOUSE_OVER, mouseOver);
		mc.stage.addEventListener(MouseEvent.MOUSE_OUT, mouseOut);
		img.mouseChildren = false;
		
		for (f in features) {
			mc.addChild(f);
		}
		
		cursor = flash.Lib.attach("cursor");
		cursor.mouseEnabled = false;
		cursor.mouseChildren = false;
		cursor.visible = false;
		mc.addChild(cursor);
		
		acceleration = 0.05;
		
		currPos = 0;
		currScale = 0.5;
		speed = Math.sqrt(2 * acceleration * numFrames * 4.0);
		Util.scaleAroundCenter(img, new Point(img.width / 2, img.height / 2), currScale, currScale);
		
		blurFilter = new BlurFilter(20.0, 1.0);
		
		timer = new Timer(10 /* milliseconds */);
		timer.addEventListener(TimerEvent.TIMER, timerHandler);
		timer.start();
		
	}
	
	private function mouseOver(event:MouseEvent):Void {
		cursor.visible = true;
		Mouse.hide();
	}
	
	private function mouseOut(event:MouseEvent):Void {
		cursor.visible = false;
		Mouse.show();
	}
	
	private function mouseDown(event:MouseEvent):Void {
		dragging = true;
		fromX = event.stageX;
		fromY = event.stageY;
		fromFrame = currFrame;
		fromTime = flash.Lib.getTimer();
		
		prevX = event.stageX;
		prevTime = fromTime;
		deltaX = 0;
		deltaTime = 0;
	}

	private function mouseUp(event:MouseEvent):Void {
		currPos = currFrame;
		dragging = false;
		
		/*
		var currTime = flash.Lib.getTimer();
		deltaX = fromX - event.stageX;
		deltaTime = currTime - fromTime;
		speed = 10 * deltaX / deltaTime / numFrames;
		if (Math.abs(speed) < 0.1) {
			speed = 0;
		}
		*/
		if (speed > 5.0) {
			speed = 5.0;
		} else if (speed < -5.0) {
			speed = -5.0;
		}
			
		img.filters = [];
	}
	
	private function setFrame(frame:Int) {
		if (frame != currFrame) {
			currFrame = frame;
			//trace("Frame: " + currFrame);
			img.gotoAndStop(currFrame+1);
			for (f in features) {
				f.updateFrame(currFrame);
			}
		}
	}
	
	private function updateBlur() {
		if (Math.abs(speed) > 0.5) {
			blurFilter.blurX = Math.abs(speed) * 5.0;
			if (blurFilter.blurX > 16) {
				blurFilter.blurX = 16;
			}
			//blurFilter.blurY = blurFilter.blurX;
			img.filters = [blurFilter];
		} else {
			img.filters = [];
		}
	}

	private function mouseMove(event:MouseEvent):Void {
		cursor.x = event.stageX - cursor.width/2;
		cursor.y = event.stageY - cursor.height/2;
		
		if (!event.buttonDown) {
			dragging = false;
		}
		if (dragging) {
			var dx:Float = fromX - event.stageX;
			var frame:Int = (fromFrame + Math.round(dx / 10)) % numFrames;
			frame = frame < 0 ? frame + numFrames : frame;
			setFrame(frame);
			
			var currTime = flash.Lib.getTimer();
			deltaX = prevX - event.stageX;
			deltaTime = currTime - prevTime;
			speed = 10 * deltaX / deltaTime / numFrames;
			//trace("Speed: " + speed);
			updateBlur();
			
			prevX = event.stageX;
			prevTime = currTime;
		}
	}
	
	private function timerHandler(event:TimerEvent) {
		if (dragging) {
			var currTime = flash.Lib.getTimer();
			deltaTime += currTime - prevTime;
			if(deltaTime > 0) {
				speed = 10 * deltaX / deltaTime / numFrames;
				updateBlur();
			}
		} else {
			if (speed != 0) {
				currPos += speed / 4.0;
				var frame:Int = Math.round(currPos) % numFrames;
				frame = frame < 0 ? frame + numFrames : frame;
				setFrame(frame);
				updateBlur();
				
				if (speed > 0.0) {
					speed -= acceleration;
					if (speed < 0) speed = 0;
				} else {
					speed += acceleration;
					if (speed > 0) speed = 0;
				}
			}
		}
		if (currScale < 1.0) {
			currScale += 0.01;
			img.scaleX = img.scaleY = 1;
			img.x = img.y = 0;
			Util.scaleAroundCenter(img, new Point(img.width / 2, img.height / 2), currScale, currScale);
		}
	}
}