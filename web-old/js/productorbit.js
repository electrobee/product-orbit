var orbit = {};

orbit.element = null;

orbit.setFrame = function(div, frame) {
  if(div.curr != frame) {
    div.curr = frame;
    var shiftY = div.curr * div.imageHeight;
    div.style.backgroundPosition = "0 -" + shiftY + "px";
    orbit.updateFeatures(div, true);
  }
}

orbit.mouseEnter = function(event, div) {
  orbit.updateFeatures(div, true);
}

orbit.mouseLeave = function(event, div) {
  orbit.updateFeatures(div, false);
}

orbit.mouseDown = function(event, div) {
  if(div.timer) {
    clearInterval(div.timer);
    div.timer = null;
  }
  if(div.timeout) {
    clearTimeout(div.timeout);
    div.timeout = null;
  }
  orbit.element = div;
  div.fromX = event.screenX;
  div.from = div.curr;

  var d = new Date();
  div.fromTime = d.getTime();
}

orbit.mouseUp = function(event) {
  if(!orbit.element) return;
  if(orbit.element && orbit.element != event.target && event.target.className != "feature") {
    orbit.updateFeatures(orbit.element, false);
  }

  var d = new Date();
  var time = d.getTime() - orbit.element.fromTime;

  var speed = (orbit.element.fromX - event.screenX) / time;
  var inc = 1;
  if(speed < 0.0) {
    speed = -speed;
    inc = -1;
  }
  var frame = orbit.element.curr;
  var div = orbit.element;

  var delay = function(speed) {
    return Math.round(20.0 / speed);
  };

  var func = function() {
    speed -= 0.05;
    frame = (frame + inc) % div.numImages;
    if(frame < 0) frame += div.numImages;
    orbit.setFrame(div, frame);
    if(speed > 0.1) {
      div.timeout = window.setTimeout(func, delay(speed));
    } else {
      div.timeout = null;
    }
  };

  if(speed > 0.5) {
    div.timeout = window.setTimeout(func, delay(speed));
  }

  orbit.element = null;
}

orbit.mouseMove = function(event) {
  if(orbit.element) {
    var frame = (orbit.element.from + Math.round((orbit.element.fromX - event.screenX) / 10)) % orbit.element.numImages;
    if(frame < 0) frame += orbit.element.numImages;
    orbit.setFrame(orbit.element, frame);
  }
}

orbit.updateFeatures = function(div, show) {
  $(div).children('.feature').each(function() {
    var frame = parseInt(this.id.split("_",2)[1], 10);
    if(frame == div.curr && show && !div.timer) {
      this.hidden = false;
      $(this).stop(true, false).fadeTo('fast', 0.5);
    } else {
      this.hidden = true;
      $(this).stop(true, false).fadeTo('fast', 0);
    }
  });
}

$(document).ready(function() {
  if($.browser.msie) {
    $("div.orbit").bind('selectstart',function() { return false; });
  }
  $("div.feature").tipTip({delay: 0});
  $("div.orbit").each(function() {
    this.numImages = parseInt(this.id.split("_",2)[1], 10);
    this.imageHeight = parseInt(this.style.height, 10);
    this.curr = 0;
  });
  $("div.orbit_animate").each(function() {
    var obj = this;
    this.timer = window.setInterval(function() {
      var frame = (obj.curr + 1) % obj.numImages;
      orbit.setFrame(obj, frame);
      if(frame == 0) {
        clearInterval(obj.timer);
        obj.timer = null;
      }
    }, Math.round(5000 / this.numImages));
  });

  $("div.orbit").each(function() {
    this.ontouchmove = function(event) {
      event.preventDefault();
      event.screenX = event.touches[0].screenX;
      orbit.mouseMove(event);
      return true;
    };
    this.ontouchend = function(event) {
      event.preventDefault();
      event.screenX = event.touches[0].screenX;
      orbit.mouseUp(event);
      return true;
    };
    this.ontouchstart = function(event) {
      event.preventDefault();
      event.screenX = event.touches[0].screenX;
      orbit.mouseDown(event, this);
      return true;
    };
  });
  $("div.orbit").mousedown(function(event) {
    orbit.mouseDown(event, this);
    return true;
  });
  $(document).mousemove(function(event) {
    orbit.mouseMove(event);
    return true;
  });
  $(document).mouseup(function(event) {
    orbit.mouseUp(event);
    return true;
  });
  $(document).mousedown(function(event) {
    return orbit.element == null;
  });
});
