// =================================================================================================
//
//	Starling Framework
//	Copyright Gamua GmbH. All Rights Reserved.
//
//	This program is free software. You can redistribute and/or modify it
//	in accordance with the terms of the accompanying license agreement.
//
// =================================================================================================

package starling.display
{
    import flash.errors.IllegalOperationError;
    import flash.media.Sound;
    import flash.media.SoundTransform;

    import starling.animation.IAnimatable;
    import starling.events.Event;
    import starling.textures.Texture;

    /** Dispatched whenever the movie has displayed its last frame. */
    [Event(name="complete", type="starling.events.Event")]

    /** A MovieClip is a simple way to display an animation depicted by a list of textures.
     *  
     *  <p>Pass the frames of the movie in a vector of textures to the constructor. The movie clip 
     *  will have the width and height of the first frame. If you group your frames with the help 
     *  of a texture atlas (which is recommended), use the <code>getTextures</code>-method of the 
     *  atlas to receive the textures in the correct (alphabetic) order.</p> 
     *  
     *  <p>You can specify the desired framerate via the constructor. You can, however, manually 
     *  give each frame a custom duration. You can also play a sound whenever a certain frame 
     *  appears, or execute a callback (a "frame action").</p>
     *  
     *  <p>The methods <code>play</code> and <code>pause</code> control playback of the movie. You
     *  will receive an event of type <code>Event.COMPLETE</code> when the movie finished
     *  playback. If the movie is looping, the event is dispatched once per loop.</p>
     *  
     *  <p>As any animated object, a movie clip has to be added to a juggler (or have its 
     *  <code>advanceTime</code> method called regularly) to run. The movie will dispatch 
     *  an event of type "Event.COMPLETE" whenever it has displayed its last frame.</p>
     *  
     *  @see starling.textures.TextureAtlas
     */    
    public class MovieClip extends Image implements IAnimatable
    {
        private var _frames:Vector.<MovieClipFrame>;
        private var _defaultFrameDuration:Number;
        private var _currentTime:Number;
        private var _currentFrameID:int;
        private var _loop:Boolean;
        private var _playing:Boolean;
        private var _muted:Boolean;
        private var _wasStopped:Boolean;
        private var _soundTransform:SoundTransform;

        /** Creates a movie clip from the provided textures and with the specified default framerate.
         *  The movie will have the size of the first frame. */  
        public function MovieClip(textures:Vector.<Texture>, fps:Number=12)
        {
            if (textures.length > 0)
            {
                super(textures[0]);
                init(textures, fps);
            }
            else
            {
                throw new ArgumentError("Empty texture array");
            }
        }
        
        private function init(textures:Vector.<Texture>, fps:Number):void
        {
            if (fps <= 0) throw new ArgumentError("Invalid fps: " + fps);
            var numFrames:int = textures.length;
            
            _defaultFrameDuration = 1.0 / fps;
            _loop = true;
            _playing = true;
            _currentTime = 0.0;
            _currentFrameID = 0;
            _wasStopped = true;
            _frames = new <MovieClipFrame>[];

            for (var i:int=0; i<numFrames; ++i)
                _frames[i] = new MovieClipFrame(
                        textures[i], _defaultFrameDuration, _defaultFrameDuration * i);
        }
        
        // frame manipulation
        
        /** Adds an additional frame, optionally with a sound and a custom duration. If the 
         *  duration is omitted, the default framerate is used (as specified in the constructor). */   
        public function addFrame(texture:Texture, sound:Sound=null, duration:Number=-1):void
        {
            addFrameAt(numFrames, texture, sound, duration);
        }
        
        /** Adds a frame at a certain index, optionally with a sound and a custom duration. */
        public function addFrameAt(frameID:int, texture:Texture, sound:Sound=null, 
                                   duration:Number=-1):void
        {
            if (frameID < 0 || frameID > numFrames) throw new ArgumentError("Invalid frame id");
            if (duration < 0) duration = _defaultFrameDuration;

            var frame:MovieClipFrame = new MovieClipFrame(texture, duration);
            frame.sound = sound;
            _frames.insertAt(frameID, frame);

            if (frameID == numFrames)
            {
                var prevStartTime:Number = frameID > 0 ? _frames[frameID - 1].startTime : 0.0;
                var prevDuration:Number  = frameID > 0 ? _frames[frameID - 1].duration  : 0.0;
                frame.startTime = prevStartTime + prevDuration;
            }
            else
                updateStartTimes();
        }
        
        /** Removes the frame at a certain ID. The successors will move down. */
        public function removeFrameAt(frameID:int):void
        {
            if (frameID < 0 || frameID >= numFrames) throw new ArgumentError("Invalid frame id");
            if (numFrames == 1) throw new IllegalOperationError("Movie clip must not be empty");

            _frames.removeAt(frameID);

            if (frameID != numFrames)
                updateStartTimes();
        }
        
        /** Returns the texture of a certain frame. */
        public function getFrameTexture(frameID:int):Texture
        {
            if (frameID < 0 || frameID >= numFrames) throw new ArgumentError("Invalid frame id");
            return _frames[frameID].texture;
        }
        
        /** Sets the texture of a certain frame. */
        public function setFrameTexture(frameID:int, texture:Texture):void
        {
            if (frameID < 0 || frameID >= numFrames) throw new ArgumentError("Invalid frame id");
            _frames[frameID].texture = texture;
        }
        
        /** Returns the sound of a certain frame. */
        public function getFrameSound(frameID:int):Sound
        {
            if (frameID < 0 || frameID >= numFrames) throw new ArgumentError("Invalid frame id");
            return _frames[frameID].sound;
        }
        
        /** Sets the sound of a certain frame. The sound will be played whenever the frame 
         *  is displayed. */
        public function setFrameSound(frameID:int, sound:Sound):void
        {
            if (frameID < 0 || frameID >= numFrames) throw new ArgumentError("Invalid frame id");
            _frames[frameID].sound = sound;
        }

        /** Returns the method that is executed at a certain frame. */
        public function getFrameAction(frameID:int):Function
        {
            if (frameID < 0 || frameID >= numFrames) throw new ArgumentError("Invalid frame id");
            return _frames[frameID].action;
        }

        /** Sets an action that will be executed whenever a certain frame is reached. */
        public function setFrameAction(frameID:int, action:Function):void
        {
            if (frameID < 0 || frameID >= numFrames) throw new ArgumentError("Invalid frame id");
            _frames[frameID].action = action;
        }
        
        /** Returns the duration of a certain frame (in seconds). */
        public function getFrameDuration(frameID:int):Number
        {
            if (frameID < 0 || frameID >= numFrames) throw new ArgumentError("Invalid frame id");
            return _frames[frameID].duration;
        }
        
        /** Sets the duration of a certain frame (in seconds). */
        public function setFrameDuration(frameID:int, duration:Number):void
        {
            if (frameID < 0 || frameID >= numFrames) throw new ArgumentError("Invalid frame id");
            _frames[frameID].duration = duration;
            updateStartTimes();
        }

        /** Reverses the order of all frames, making the clip run from end to start.
         *  Makes sure that the currently visible frame stays the same. */
        public function reverseFrames():void
        {
            _frames.reverse();
            _currentTime = totalTime - _currentTime;
            _currentFrameID = numFrames - _currentFrameID - 1;
            updateStartTimes();
        }
        
        // playback methods
        
        /** Starts playback. Beware that the clip has to be added to a juggler, too! */
        public function play():void
        {
            _playing = true;
        }
        
        /** Pauses playback. */
        public function pause():void
        {
            _playing = false;
        }
        
        /** Stops playback, resetting "currentFrame" to zero. */
        public function stop():void
        {
            _playing = false;
            _wasStopped = true;
            currentFrame = 0;
        }

        // helpers
        
        private function updateStartTimes():void
        {
            var numFrames:int = this.numFrames;
            var prevFrame:MovieClipFrame = _frames[0];
            prevFrame.startTime = 0;
            
            for (var i:int=1; i<numFrames; ++i)
            {
                _frames[i].startTime = prevFrame.startTime + prevFrame.duration;
                prevFrame = _frames[i];
            }
        }

        // IAnimatable

        /** @inheritDoc */
        public function advanceTime(passedTime:Number):void
        {
            if (!_playing) return;

            // The tricky part in this method is that whenever a callback is executed
            // (a frame action or a 'COMPLETE' event handler), that callback might modify the clip.
            // Thus, we have to start over with the remaining time whenever that happens.

            var frame:MovieClipFrame = _frames[_currentFrameID];

            if (_wasStopped)
            {
                // if the clip was stopped and started again,
                // sound and action of this frame need to be repeated.

                _wasStopped = false;
                frame.playSound(_soundTransform);

                if (frame.action != null)
                {
                    frame.executeAction(this, _currentFrameID);
                    advanceTime(passedTime);
                    return;
                }
            }

            if (_currentTime == totalTime)
            {
                if (_loop)
                {
                    _currentTime = 0.0;
                    _currentFrameID = 0;
                    frame = _frames[0];
                    frame.playSound(_soundTransform);
                    texture = frame.texture;

                    if (frame.action != null)
                    {
                        frame.executeAction(this, _currentFrameID);
                        advanceTime(passedTime);
                        return;
                    }
                }
                else return;
            }

            var finalFrameID:int = _frames.length - 1;
            var restTimeInFrame:Number = frame.duration - _currentTime + frame.startTime;
            var dispatchCompleteEvent:Boolean = false;
            var frameAction:Function = null;
            var previousFrameID:int = _currentFrameID;
            var changedFrame:Boolean;

            while (passedTime >= restTimeInFrame)
            {
                changedFrame = false;
                passedTime -= restTimeInFrame;
                _currentTime = frame.startTime + frame.duration;

                if (_currentFrameID == finalFrameID)
                {
                    if (hasEventListener(Event.COMPLETE))
                    {
                        dispatchCompleteEvent = true;
                    }
                    else if (_loop)
                    {
                        _currentTime = 0;
                        _currentFrameID = 0;
                        changedFrame = true;
                    }
                    else return;
                }
                else
                {
                    _currentFrameID += 1;
                    changedFrame = true;
                }

                frame = _frames[_currentFrameID];
                frameAction = frame.action;

                if (changedFrame)
                    frame.playSound(_soundTransform);

                if (dispatchCompleteEvent)
                {
                    texture = frame.texture;
                    dispatchEventWith(Event.COMPLETE);
                    advanceTime(passedTime);
                    return;
                }
                else if (frameAction != null)
                {
                    texture = frame.texture;
                    frame.executeAction(this, _currentFrameID);
                    advanceTime(passedTime);
                    return;
                }

                restTimeInFrame = frame.duration;

                // prevent a mean floating point problem (issue #851)
                if (passedTime + 0.0001 > restTimeInFrame && passedTime - 0.0001 < restTimeInFrame)
                    passedTime = restTimeInFrame;
            }

            if (previousFrameID != _currentFrameID)
                texture = _frames[_currentFrameID].texture;

            _currentTime += passedTime;
        }
        
        // properties

        /** The total number of frames. */
        public function get numFrames():int { return _frames.length; }
        
        /** The total duration of the clip in seconds. */
        public function get totalTime():Number 
        {
            var lastFrame:MovieClipFrame = _frames[_frames.length-1];
            return lastFrame.startTime + lastFrame.duration;
        }
        
        /** The time that has passed since the clip was started (each loop starts at zero). */
        public function get currentTime():Number { return _currentTime; }
        public function set currentTime(value:Number):void
        {
            if (value < 0 || value > totalTime) throw new ArgumentError("Invalid time: " + value);

            var lastFrameID:int = _frames.length - 1;
            _currentTime = value;
            _currentFrameID = 0;

            while (_currentFrameID < lastFrameID && _frames[_currentFrameID + 1].startTime <= value)
                ++_currentFrameID;

            var frame:MovieClipFrame = _frames[_currentFrameID];
            texture = frame.texture;
        }

        /** Indicates if the clip should loop. @default true */
        public function get loop():Boolean { return _loop; }
        public function set loop(value:Boolean):void { _loop = value; }
        
        /** If enabled, no new sounds will be started during playback. Sounds that are already
         *  playing are not affected. */
        public function get muted():Boolean { return _muted; }
        public function set muted(value:Boolean):void { _muted = value; }

        /** The SoundTransform object used for playback of all frame sounds. @default null */
        public function get soundTransform():SoundTransform { return _soundTransform; }
        public function set soundTransform(value:SoundTransform):void { _soundTransform = value; }

        /** The index of the frame that is currently displayed. */
        public function get currentFrame():int { return _currentFrameID; }
        public function set currentFrame(value:int):void
        {
            if (value < 0 || value >= numFrames) throw new ArgumentError("Invalid frame id");
            currentTime = _frames[value].startTime;
        }
        
        /** The default number of frames per second. Individual frames can have different 
         *  durations. If you change the fps, the durations of all frames will be scaled 
         *  relatively to the previous value. */
        public function get fps():Number { return 1.0 / _defaultFrameDuration; }
        public function set fps(value:Number):void
        {
            if (value <= 0) throw new ArgumentError("Invalid fps: " + value);
            
            var newFrameDuration:Number = 1.0 / value;
            var acceleration:Number = newFrameDuration / _defaultFrameDuration;
            _currentTime *= acceleration;
            _defaultFrameDuration = newFrameDuration;
            
            for (var i:int=0; i<numFrames; ++i)
                _frames[i].duration *= acceleration;

            updateStartTimes();
        }
        
        /** Indicates if the clip is still playing. Returns <code>false</code> when the end 
         *  is reached. */
        public function get isPlaying():Boolean 
        {
            if (_playing)
                return _loop || _currentTime < totalTime;
            else
                return false;
        }

        /** Indicates if a (non-looping) movie has come to its end. */
        public function get isComplete():Boolean
        {
            return !_loop && _currentTime >= totalTime;
        }
    }
}

import flash.media.Sound;
import flash.media.SoundTransform;

import starling.display.MovieClip;
import starling.textures.Texture;

class MovieClipFrame
{
    public function MovieClipFrame(texture:Texture, duration:Number=0.1,  startTime:Number=0)
    {
        this.texture = texture;
        this.duration = duration;
        this.startTime = startTime;
    }

    public var texture:Texture;
    public var sound:Sound;
    public var duration:Number;
    public var startTime:Number;
    public var action:Function;

    public function playSound(transform:SoundTransform):void
    {
        if (sound) sound.play(0, 0, transform);
    }

    public function executeAction(movie:MovieClip, frameID:int):void
    {
        if (action != null)
        {
            var numArgs:int = action.length;

            if (numArgs == 0) action();
            else if (numArgs == 1) action(movie);
            else if (numArgs == 2) action(movie, frameID);
            else throw new Error("Frame actions support zero, one or two parameters: " +
                    "movie:MovieClip, frameID:int");
        }
    }
}