 (function ($) {
  var hideEffect = { opacity: 0 };
  var showEffect = { opacity: 1 };
  
  if (!$.support.opacity) {
  hideEffect = { };
  showEffect = { };
  }
  
  var Orbiter = function ($sourceElement, options) {
  this.$sourceElement = $sourceElement;
  this.options = options;
  
  this.$sourceElement
  .css({ cursor: this.options.loadTrigger === 'click' ? 'pointer' : undefined })
  .disableSelection();
  
  this.$container = $(document.createElement('div'))
  .addClass('orbiter')
  .css({
       display: 'inline-block',
       position: 'relative'
       })
  .disableSelection()
  .data('orbiter', this);
  
  this.$featureBoxContainer = $(document.createElement('div'))
  .css({
       display: 'block',
       position: 'absolute',
       left: 0,
       right: 0,
       top: 0,
       bottom: 0
       })
  .hide()
  .css(hideEffect)
  .gxInit({ queue: 'cancel' })
  .appendTo(this.$container);
  
  this.rotationCount = options.rotationCount;
  this.rotationsPerFrame = options.rotationsPerFrame;
  this.pixelsPerRotation = options.pixelsPerRotation;
  this.features = this.options.features;
  
  this.frameImages = [ ];
  this.featureBoxes = { };
  
  this.isLoaded = false;
  
  this.bind();
  };
  
  Orbiter.prototype = {
  updateViewer: function () {
  throw 'Implement updateViewer in a subclass';
  },
  
  getViewer$: function () {
  throw 'Implement getViewer$ i na subclass';
  },
  
  bind: function () {
  var orbiter = this;
  
  function isLeftMouseButton(e) {
  if ($.browser.msie) {
  return e.button === 1;
  }
  
  return e.button === 0;
  }
  
  function onDown(e) {
  if (isLeftMouseButton(e)) {
  orbiter.down(e.screenX, e.screenY);
  $(document).one('mouseup', onUp);
  $(document).bind('mousemove', onMove);
  }
  }
  
  function onMove(e) {
  if (isLeftMouseButton(e)) {
  orbiter.move(e.screenX, e.screenY);
  }
  }
  
  function onUp(e) {
  if (isLeftMouseButton(e)) {
  orbiter.up(e.screenX, e.screenY);
  $(document).unbind('mousemove', onMove);
  }
  }
  
  function onTouchDown(e) {
  // No, this isn't American football
  e.preventDefault();
  
  orbiter.down(e.targetTouches[0].screenX, e.targetTouches[0].screenY);
  
  return true;
  }
  
  function onTouchMove(e) {
  e.preventDefault();
  
  orbiter.move(e.targetTouches[0].screenX, e.targetTouches[0].screenY);
  
  return true;
  }
  
  function onTouchUp(e) {
  // FIXME what if they're still touching?
  e.preventDefault();
  
  orbiter.up(e.changedTouches[0].screenX, e.changedTouches[0].screenY);
  
  return true;
  }
  
  this.$container.bind('mousedown', onDown);
  
  // jQuery binding doesn't work for some reason
  // TODO Submit a bug report...
  this.$container[0].ontouchstart = onTouchDown;
  this.$container[0].ontouchmove  = onTouchMove;
  this.$container[0].ontouchend   = onTouchUp;
  
  // This is required for some reason...
  if (document.addEventListener != undefined && document.addEventListener != null) {
    document.addEventListener('touchstart', function() {}, false);
  }
  
  this.$container.bind('mouseenter', function () {
                       orbiter.$featureBoxContainer
                       .show()
                       .gx(showEffect, orbiter.options.featureMouseInTime, orbiter.options.featureMouseInEasing);
                       });
  
  this.$container.bind('mouseleave', function () {
                       orbiter.$featureBoxContainer
                       .gx(hideEffect, orbiter.options.featureMouseOutTime, orbiter.options.featureMouseOutEasing, function () {
                           orbiter.$featureBoxContainer.hide();
                           });
                       });
  },
  
  load: function () {
  if (this.isLoaded) {
  return;
  }
  
  this.showLoading();
  
  var orbiter = this;
  
  this.loadAllFrames(function () {
                     orbiter.isLoaded = true;
                     
                     orbiter.setRotation(0);
                     
                     var $viewer = orbiter.getViewer$()
                     .appendTo(orbiter.$container);
                     
                     orbiter.$container.css({
                                            width: orbiter.$sourceElement.width(),
                                            height: orbiter.$sourceElement.height()
                                            });
                     
                     orbiter.hideLoading(function () {
                                         orbiter.$sourceElement.replaceWith(orbiter.$container);
                                         });
                     });
  },
  
  showLoading: function () {
  // TODO Maybe split this into its own class or something
  
  if (!this.options.showLoading) {
  return;
  }
  
  var orbiter = this;
  
  this.$sourceElement.css({ cursor: 'wait' });
  
  this.loadingTimeout = window.setTimeout(function () {
                                          var region = orbiter.$sourceElement.offset();
                                          region.width = orbiter.$sourceElement.width();
                                          region.height = orbiter.$sourceElement.height();
                                          
                                          var loaderImageSize = {
                                          width: 32,
                                          height: 32
                                          };
                                          
                                          orbiter.$sourceElement.animate({
                                                                         opacity: 0.5
                                                                         }, 500);
                                          
                                          orbiter.$loading = $(document.createElement('img'))
                                          .attr('src', 'images/loader.gif')
                                          .css({
                                               position: 'absolute',
                                               left: region.left + (region.width  - loaderImageSize.width ) / 2,
                                               top:  region.top  + (region.height - loaderImageSize.height) / 2
                                               })
                                          .appendTo(document.body);
                                          }, 0);
  },
  
  hideLoading: function (callback) {
  if (!this.options.showLoading) {
  callback();
  
  return;
  }
  
  if (this.loadingTimeout) {
  window.clearTimeout(this.loadingTimeout);
  }
  
  this.$sourceElement
  .css({ cursor: '' })
  .animate({
           opacity: 1
           }, 200, callback);
  
  if (this.$loading) {
  this.$loading.remove();
  
  this.$loading = null;
  }
  },
  
  loadAllFrames: function (callback) {
  var totalFrames = Math.ceil(this.rotationCount / this.rotationsPerFrame);
  var framesRemaining = totalFrames;
  
  function frameLoaded() {
  --framesRemaining;
  
  if (framesRemaining <= 0) {
  callback();
  }
  }
  
  var frame;
  
  for (frame = 0; frame < totalFrames; ++frame) {
  this.loadFrameImage(frame, frameLoaded);
  }
  },
  
  wrapRotation: function (rotation) {
  var rotationCount = this.rotationCount;
  
  if (rotationCount === 0) {
  // We could be here a while...
  return 0;
  }
  
  while (rotation >= rotationCount) {
  rotation -= rotationCount;
  }
  
  while (rotation < 0) {
  rotation += rotationCount;
  }
  
  return rotation;
  },
  
  setRotation: function (rotation) {
  if (parseInt(rotation, 10) != rotation) {
  // (Loose comparison intentional)
  alert ("rotation: " + rotation);
  throw 'Rotation number must be an integer';
  }
  
  rotation = parseInt(rotation, 10);
  
  if (rotation < 0 || rotation >= this.rotationCount) {
  throw 'Rotation number out of range';
  }
  
  if (rotation === this.currentRotation) {
  return;
  }
  
  this.currentRotation = rotation;
  
  this.updateViewer();
  this.updateFeatureBoxes();
  },
  
  updateFeatureBoxes: function (rotation) {
  var orbiter = this;
  var currentFeatures = this.getFeatures(this.currentRotation);
  
  var currentFeatureNames = $(currentFeatureNames).map(function () {
                                                       return this.name;
                                                       }).get();
  
  // Hide inactive boxes
  $.each(this.featureBoxes, function (text, $element) {
         if ($(currentFeatureNames).index(text) < 0) {
         if ($element.isFadingOut) {
         return;
         }
         
         $element.gx(hideEffect, orbiter.options.featureFadeOutTime, orbiter.options.featureFadeOutEasing, function () {
                     delete orbiter.featureBoxes[text];
                     
                     $element.hide();
                     });
         
         $element.isFadingOut = true;
         }
         });
  
  $(currentFeatures).each(function () {
                          var $element = orbiter.featureBoxes[this.name];
                          
                          if ($element) {
                          $element.isFadingOut = false;
                          
                          $element.show();
                          
                          $element.gx($.extend({ }, showEffect, this.position), orbiter.options.featureFadeInTime, orbiter.options.featureFadeInEasing);
                          } else {
                          $element = $(document.createElement('div'))
                          .addClass('orbiter-feature')
                          .css(this.position)
                          .css({ position: 'absolute' })
                          .css(hideEffect)
                          .attr('title', this.name)
                          .gxInit({ queue: 'cancel' })
                          .gx(showEffect, orbiter.options.featureFadeInTime, orbiter.options.featureFadeInEasing)
                          .appendTo(orbiter.$featureBoxContainer);
                          
                          toolTipIt$($element)
                          .addClass('feature-tool-tip');
                          }
                          
                          orbiter.featureBoxes[this.name] = $element;
                          });
  },
  
  getFeatures: function (rotation) {
  // jQuery is retarded and won't let us filter on an object.  =|
  var features = [ ];
  
  $.each(this.features, function (key, value) {
         if (value[rotation]) {
         features.push({
                       name: key,
                       position: value[rotation]
                       });
         }
         });
  
  return features;
  },
  
  loadFrameImage: function (frame, callback) {
  var img = this.frameImages[frame];
  
  if (img) {
  if (img.data('callbacks')) {
  img.data('callbacks').push(callback);
  } else {
  callback(img);
  }
  
  return;
  }
  
  var img = this.options.frameImage;
  var imgSrc;
  
  if (typeof img === 'function') {
  img = img(this.$sourceElement, frame);
  }
  
  if (typeof img === 'string') {
  imgSrc = img;
  img = new Image();
  }
  
  if (img.tagName && img.tagName === 'IMG') {
  img = $(img);
  }
  
  if (!(img instanceof $)) {
  throw 'image should be a jQuery object';
  }
  
  this.frameImages[frame] = img;
  
  img.data('callbacks', [ callback ]);
  
  img.load(function () {
           $(img.data('callbacks')).each(function () {
                                         this(img);
                                         });
           });
  
  // IE needs src to be set after the load handler is attached
  // else cached images do not fire the load event.
  if (imgSrc) {
  img.attr('src', imgSrc);
  }
  },
  
  down: function (x, y) {
  this.isDown = true;
  
  this.startX = x;
  this.startY = y;
  this.startTick = this.getTimeTick();
  this.startRotation = this.currentRotation;
  
  this.endMomentum();
  },
  
  move: function (x, y) {
  if (!this.isDown) {
  return;
  }
  
  var dx = this.startX - x;
  var rotations = Math.round(dx / this.pixelsPerRotation);
  
  this.setRotation(this.wrapRotation(this.startRotation + rotations));
  },
  
  up: function (x, y) {
  if (!this.isDown) {
  return;
  }
  
  this.isDown = false;
  
  this.startMomentum((this.startX - x) / (this.getTimeTick() - this.startTick));
  },
  
  getTimeTick: function () {
  return (new Date()).getTime();
  },
  
  startMomentum: function (force) {
  if (force === 0) {
  return;
  }
  
  var rotation = this.currentRotation;
  var speed = Math.min(2.5, Math.abs(force));
  var direction = Math.abs(force) / force;
  
  if (speed < 1) {
  return;
  }
  
  var orbiter = this;
  
  function nextTick() {
  speed -= 0.05;
  
  if (speed < 0) {
  orbiter.endMomentum();
  
  return;
  }
  
  rotation += direction;
  
  orbiter.setRotation(orbiter.wrapRotation(rotation));
  
  var time = 20 / speed;
  
  if (time > 400) {
  return;
  }
  
  orbiter.momentumTimer = window.setTimeout(nextTick, time);
  }
  
  nextTick();
  },
  
  endMomentum: function () {
  window.clearTimeout(this.momentumTimer);
  }
  };
  
  function CssOrbiter($sourceElement, options) {
  var orbiter = new Orbiter($sourceElement, options);
  
  var $div = null;
  
  orbiter.getViewer$ = function () {
  if ($div) {
  return $div;
  }
  
  $div = $(document.createElement('div'))
  .width(this.$sourceElement.width())
  .height(this.$sourceElement.height());
  
  return $div;
  };
  
  orbiter.updateViewer = function () {
  var $div = this.getViewer$();
  
  var rotation = this.currentRotation;
  var frame = Math.floor(rotation / this.rotationsPerFrame);
  var rotationInFrame = rotation % this.rotationsPerFrame;
  
  var x = 0;
  var y = rotationInFrame * $div.height();
  
  var $frameImage = this.frameImages[frame];
  
  $div.css({
           backgroundImage: 'url(' + escape($frameImage.attr('src')) + ')',
           backgroundPosition: '0 ' + y + 'px'
           });
  };
  
  return orbiter;
  }
  
  function CanvasOrbiter($sourceElement, options) {
  var orbiter = new Orbiter($sourceElement, options);
  
  var $canvas = null;
  
  orbiter.getViewer$ = function () {
  if ($canvas) {
  return $canvas;
  }
  
  var canvas = document.createElement('canvas');
  
  canvas.width = this.$sourceElement.width();
  canvas.height = this.$sourceElement.height();
  
  $canvas = $(canvas);
  
  return $canvas;
  };
  
  orbiter.updateViewer = function () {
  var rotation = this.currentRotation;
  var frame = Math.floor(rotation / this.rotationsPerFrame);
  var rotationInFrame = rotation % this.rotationsPerFrame;
  
  var context = this.getViewer$()[0].getContext('2d');
  var width = context.canvas.width;
  var height = context.canvas.height;
  var x = 0;
  var y = rotationInFrame * height;
  
  var $frameImage = this.frameImages[frame];
  
  context.drawImage(
                    $frameImage[0],         // image
                    x, y, width, height,    // source
                    0, 0, width, height     // destination
                    );
  };
  
  return orbiter;
  }
  
  function toolTipIt$($element) {
  var $toolTip = $(document.createElement('div'))
  .gxInit({ queue: 'cancel' })
  .css({ position: 'absolute' })
  .css(hideEffect)
  .hide();
  
  $element
  .data('toolTipIt', {
        title: $element.attr('title'),
        $toolTip: $toolTip,
        hideTimer: null
        })
  .attr('title', '')
  .bind('mouseenter', toolTipIt$.enter)
  .bind('mouseleave', toolTipIt$.leave);
  
  return $toolTip;
  }
  
  toolTipIt$.enter = function () {
  var $element = $(this);
  var data = $element.data('toolTipIt');
  
  var offset = $element.offset();
  
  if (data.hideTimer) {
  window.clearTimeout(data.hideTimer);
  data.hideTimer = null;
  }
  
  data.$toolTip
  .text(data.title)
  .show()
  .gx(showEffect, 300, 'Quad:Out');
  
  // Append before applying CSS because
  // outerHeight doesn't exist until the
  // element is added to the DOM
  data.$toolTip
  .appendTo($element)
  .css({
       top: -data.$toolTip.outerHeight() - 1   // Fix off-by-one pixel error
       })
  };
  
  toolTipIt$.leave = function () {
  var $element = $(this);
  var data = $element.data('toolTipIt');
  
  if (data.hideTimer) {
  window.clearTimeout(data.hideTimer);
  data.hideTimer = null;
  }
  
  data.hideTimer = window.setTimeout(function () {
                                     data.hideTimer = null;
                                     
                                     data.$toolTip
                                     .gx(hideEffect, 300, 'Sine', function () {
                                         data.$toolTip
                                         .css({
                                              top: 0
                                              })
                                         .remove();
                                         });
                                     }, 500);
  };
  
  function getOrbiterClass() {
  // Prefer canvas; fall back to plain CSS
  
  try {
  if (document.createElement('canvas').getContext) {
  return CanvasOrbiter;
  }
  } catch (e) {
  return CssOrbiter;
  }
  
  return CssOrbiter;
  }
  
  var orbit = function (options) {
  options = $.extend({ }, orbit.defaultOptions, options);
  
  this.each(function () {
            var $orbitee = $(this);
            
            try {
            var OrbiterClass = getOrbiterClass();
            var orbiter = new OrbiterClass($orbitee, options);
            
            switch (options.loadTrigger) {
            case 'now':
            default:
            orbiter.load();
            
            break;
            
            case 'click':
            $orbitee.one('mousedown', function () {
                         orbiter.load();
                         });
            
            break;
            
            case 'hover':
            $orbitee.one('mouseenter', function () {
                         orbiter.load();
                         });
            
            break;
            }
            
            $orbitee.data('orbiter', orbiter);
            } catch (e) {
            console.log('Exception in $.fn.orbit: ');
            console.log(e);
            }
            });
  };
  
  orbit.defaultOptions = {
  rotationCount: 32,
  
  frameImage: function ($sourceElement, frame) {
  var url = $sourceElement.attr('src');
  url = url.replace('-thumb', '-' + frame);
  url = url.replace(/#.*$/, '');
  
  return url;
  },
  
  features: { },
  
  featureFadeInTime: 300,
  featureFadeOutTime: 600,
  
  featureFadeInEasing: 'Sine:Out',
  featureFadeOutEasing: 'Sine:Out',
  
  featureMouseInTime: 500,
  featureMouseOutTime: 1000,
  
  featureMouseInEasing: 'Expo',
  featureMouseOutEasing: 'Expo',
  
  rotationsPerFrame: 1,
  pixelsPerRotation: 10,
  
  showLoading: false,
  loadTrigger: 'now'  // now, hover, click
  };
  
  $.fn.orbit = orbit;
  
  $.fn.disableSelection = function () {
  return this
  .bind('selectstart.disableSelection mousedown.disableSelection', function (e) {
        e.preventDefault();
        })
  .attr('unselectable', 'unselectable'); // IE
  };
  
  $(function () {
    $('img.auto-orbit').each(function () {
                             var $image = $(this);
                             var json = $image.attr('src').replace(/^[^#]*#/, '');
                             var options = $.parseJSON(json);
                             
                             $image.orbit(options);
                             });
    });
  }(jQuery));
