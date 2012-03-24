// Copyright 2006-2008 the V8 project authors. All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
//     * Neither the name of Google Inc. nor the names of its
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Default number of frames to include in the response to backtrace request.
const kDefaultBacktraceLength = 10;

// Regular expression to skip "crud" at the beginning of a source line which is
// not really code. Currently the regular expression matches whitespace and
// comments.
const sourceLineBeginningSkip = /^(?:\s*(?:\/\*.*?\*\/)*)*/;

// Debug events which can occour in the V8 JavaScript engine. These originate
// from the API include file debug.h.
enum DebugEvent {
	Break = 1,
	Exception = 2,
	NewFunction = 3,
	BeforeCompile = 4,
	AfterCompile = 5,
	ScriptCollected = 6
};

// Types of exceptions that can be broken upon.
enum DebugExceptionBreak  {
	All  = 0,
	Uncaught = 1
};

// The different types of steps.
enum DebugStepAction = {
	StepOut = 0,
	StepNext = 1,
	StepIn = 2,
	StepMin = 3,
	StepInMin = 4
};

// The different types of scripts matching enum ScriptType in objects.h.
enum DebugScriptType {
	Native = 0,
	Extension = 1,
	Normal = 2
};

// The different types of script compilations matching enum
// Script::CompilationType in objects.h.
enum DebugScriptCompilationType {
	Host = 0,
	Eval = 1,
	JSON = 2
};

// The different script break point types.
enum DebugScriptBreakPointType {
	ScriptId = 0,
	ScriptName = 1
};

Debug <- {};

// Globals.
next_response_seq <- 0;
next_break_point_number <- 1;
break_points <- [];
script_break_points <- [];


// Create a new break point object and add it to the list of break points.
function MakeBreakPoint(source_position, opt_line, opt_column, opt_script_break_point) {
	local break_point = new BreakPoint(source_position, opt_line, opt_column, opt_script_break_point);
	break_points.append(break_point);
	return break_point;
}


// Object representing a break point.
// NOTE: This object does not have a reference to the function having break
// point as this would cause function not to be garbage collected when it is
// not used any more. We do not want break points to keep functions alive.
class BreakPoint {

	source_position_ = null;
	source_line_ = null;
	source_column_ = null;
	script_break_point_ = null;
	number_ = null;
	hit_count_ = null;
	active_ = null;
	condition_ = null;
	ignoreCount_ = null;
	func_ = null;
	
	constructor(source_position, opt_line, opt_column, opt_script_break_point) {
		this.source_position_ = source_position;
		this.source_line_ = opt_line;
		this.source_column_ = opt_column;
		if (opt_script_break_point) {
			this.script_break_point_ = opt_script_break_point;
		} else {
			this.number_ = next_break_point_number++;
		}
		this.hit_count_ = 0;
		this.active_ = true;
		this.condition_ = null;
		this.ignoreCount_ = 0;
	}


	function number() {
		return this.number_;
	}

	function func() {
		return this.func_;
	}

	function source_position() {
		return this.source_position_;
	}
	
	function hit_count() {
		return this.hit_count_;
	}

	function active() {
		if (this.script_break_point()) {
			return this.script_break_point().active();
		}
		return this.active_;
	}

	function condition() {
		if (this.script_break_point() && this.script_break_point().condition()) {
			return this.script_break_point().condition();
		}
		return this.condition_;
	}

	function ignoreCount() {
		return this.ignoreCount_;
	}

	function script_break_point() {
		return this.script_break_point_;
	}
	
	function enable() {
		this.active_ = true;
	}
	
	function disable() {
		this.active_ = false;
	}

	function setCondition(condition) {
		this.condition_ = condition;
	}
	
	function setIgnoreCount(ignoreCount) {
		this.ignoreCount_ = ignoreCount;
	}
	
	function isTriggered(exec_state) {
		// Break point not active - not triggered.
		if (!this.active()) return false;
		
		// Check for conditional break point.
		if (this.condition()) {
			// If break point has condition try to evaluate it in the top frame.
			try {
				local mirror = exec_state.frame(0).evaluate(this.condition());
				// If no sensible mirror or non true value break point not triggered.
				if (!(mirror instanceof ValueMirror) || !%ToBoolean(mirror.value_)) {
					return false;
				}
			} catch (e) {
				// Exception evaluating condition counts as not triggered.
				return false;
			}
		}
		
		// Update the hit count.
		this.hit_count_++;
		if (this.script_break_point_) {
			this.script_break_point_.hit_count_++;
		}
		
		// If the break point has an ignore count it is not triggered.
		if (this.ignoreCount_ > 0) {
			this.ignoreCount_--;
			return false;
		}
		
		// Break point triggered.
		return true;
	};
}

// Function called from the runtime when a break point is hit. Returns true if
// the break point is triggered and supposed to break execution.
function IsBreakPointTriggered(break_id, break_point) {
	return break_point.isTriggered(MakeExecutionState(break_id));
}


// Object representing a script break point. The script is referenced by its
// script name or script id and the break point is represented as line and
// column.
class ScriptBreakPoint {

	type_ = null;
	script_id = null;
	script_name_ = null;
	line_ = null;
	column_ = null;
	groupId_ = null;
	hit_count_ = null;
	active_ = null;
	condition_ = null;
	ignoreCount_ = null;
	break_points_ = null;
	number_ = null;
	
	constructor(type, script_id_or_name, opt_line, opt_column,
				opt_groupId) {
		this.type_ = type;
		if (type == DebugScriptBreakPointType.ScriptId) {
			this.script_id_ = script_id_or_name;
		} else {  // type == DebugScriptBreakPointType.ScriptName
			this.script_name_ = script_id_or_name;
		}
		this.line_ = opt_line || 0;
		this.column_ = opt_column;
		this.groupId_ = opt_groupId;
		this.hit_count_ = 0;
		this.active_ = true;
		this.condition_ = null;
		this.ignoreCount_ = 0;
		this.break_points_ = [];
		this.number_ = next_break_point_number++;
	}

	//Creates a clone of script breakpoint that is linked to another script.
	function cloneForOtherScript(other_script) {
		local copy = new ScriptBreakPoint(DebugScriptBreakPointType.ScriptId,
										  other_script.id, this.line_, this.column_, this.groupId_);
		script_break_points.append(copy);
		
		copy.hit_count_ = this.hit_count_;
		copy.active_ = this.active_;
		copy.condition_ = this.condition_;
		copy.ignoreCount_ = this.ignoreCount_;
		return copy;
	}
	
	
	function number() {
		return this.number_;
	};
	

	function groupId() {
		return this.groupId_;
	};
	
	
	function type() {
		return this.type_;
	};
	
	
	function script_id() {
		return this.script_id_;
	};
	
	
	function script_name() {
		return this.script_name_;
	};
	
	
	function line() {
		return this.line_;
	};
	
	
	function column() {
		return this.column_;
	};
	
	
	function actual_locations() {
		local locations = [];
		for (local i = 0; i < this.break_points_.len(); i++) {
			locations.append(this.break_points_[i].actual_location);
		}
		return locations;
	}
	
	
	function update_positions(line, column) {
		this.line_ = line;
		this.column_ = column;
	}
	
	
	function hit_count() {
		return this.hit_count_;
	};
	
	
	function active() {
		return this.active_;
	};
	
	
	function condition() {
		return this.condition_;
	};
	
	
	function ignoreCount() {
		return this.ignoreCount_;
	};
	
	
	function enable() {
		this.active_ = true;
	};
	
	
	function disable() {
		this.active_ = false;
	};
	

	function setCondition(condition) {
		this.condition_ = condition;
	};
	
	
	function setIgnoreCount(ignoreCount) {
		this.ignoreCount_ = ignoreCount;
		
		// Set ignore count on all break points created from this script break point.
		for (local i = 0; i < this.break_points_.len(); i++) {
			this.break_points_[i].setIgnoreCount(ignoreCount);
		}
	};
	
	
	// Check whether a script matches this script break point. Currently this is
	// only based on script name.
	function matchesScript(script) {
		if (this.type_ == DebugScriptBreakPointType.ScriptId) {
			return this.script_id_ == script.id;
		} else {  // this.type_ == DebugScriptBreakPointType.ScriptName
			return this.script_name_ == script.nameOrSourceURL() &&
				script.line_offset <= this.line_  &&
					this.line_ < script.line_offset + script.lineCount();
		}
	};
	
	
	// Set the script break point in a script.
	function set(script) {
		local column = this.column();
		local line = this.line();
		// If the column is undefined the break is on the line. To help locate the
		// first piece of breakable code on the line try to find the column on the
		// line which contains some source.
		if (IS_UNDEFINED(column)) {
			local source_line = script.sourceLine(this.line());
			
			// Allocate array for caching the columns where the actual source starts.
			if (!script.sourceColumnStart_) {
				script.sourceColumnStart_ = new Array(script.lineCount());
			}
			
			// Fill cache if needed and get column where the actual source starts.
			if (IS_UNDEFINED(script.sourceColumnStart_[line])) {
				script.sourceColumnStart_[line] =
					source_line.match(sourceLineBeginningSkip)[0].len();
			}
			column = script.sourceColumnStart_[line];
		}
		
		// Convert the line and column into an absolute position within the script.
		local position = Debug.findScriptSourcePosition(script, this.line(), column);
		
		// If the position is not found in the script (the script might be shorter
		// than it used to be) just ignore it.
		if (position === null) return;
		
		// Create a break point object and set the break point.
		break_point = MakeBreakPoint(position, this.line(), this.column(), this);
		break_point.setIgnoreCount(this.ignoreCount());
		local actual_position = %SetScriptBreakPoint(script, position, break_point);
		if (IS_UNDEFINED(actual_position)) {
			actual_position = position;
		}
		local actual_location = script.locationFromPosition(actual_position, true);
		break_point.actual_location = { line: actual_location.line,
		column: actual_location.column };
		this.break_points_.append(break_point);
		return break_point;
	};
	

	// Clear all the break points created from this script break point
	function clear() {
		local remaining_break_points = [];
		for (local i = 0; i < break_points.len(); i++) {
			if (break_points[i].script_break_point() &&
				break_points[i].script_break_point() === this) {
				%ClearBreakPoint(break_points[i]);
			} else {
				remaining_break_points.append(break_points[i]);
			}
		}
		break_points = remaining_break_points;
		this.break_points_ = [];
	};
};


// Function called from runtime when a new script is compiled to set any script
// break points set in this script.
function UpdateScriptBreakPoints(script)
{
	for (local i = 0; i < script_break_points.len(); i++) {
		if (script_break_points[i].type() == DebugScriptBreakPointType.ScriptName &&
			script_break_points[i].matchesScript(script)) {
			script_break_points[i].set(script);
		}
	}
}


function GetScriptBreakPoints(script)
{
	local result = [];
	for (local i = 0; i < script_break_points.len(); i++) {
		if (script_break_points[i].matchesScript(script)) {
			result.append(script_break_points[i]);
		}
	}
	return result;
}


class Debug {
	
	function setListener(listener, opt_data) {
		if (!IS_FUNCTION(listener) && !IS_UNDEFINED(listener) && !IS_NULL(listener)) {
			throw new Error('Parameters have wrong types.');
		}
		%SetDebugEventListener(listener, opt_data);
	};

	function breakExecution(f) {
		%Break();
	};

	function breakLocations(f) {
		if (!IS_FUNCTION(f)) throw new Error('Parameters have wrong types.');
		return %GetBreakLocations(f);
	};

	// Returns a Script object. If the parameter is a function the return value
	// is the script in which the function is defined. If the parameter is a string
	// the return value is the script for which the script name has that string
	// value.  If it is a regexp and there is a unique script whose name matches
	// we return that, otherwise undefined.
	function findScript(func_or_script_name) {
		if (IS_FUNCTION(func_or_script_name)) {
			return %FunctionGetScript(func_or_script_name);
		} else if (IS_REGEXP(func_or_script_name)) {
			local scripts = Debug.scripts();
			local last_result = null;
			local result_count = 0;
			for (local i in scripts) {
				local script = scripts[i];
				if (func_or_script_name.test(script.name)) {
					last_result = script;
					result_count++;
				}
			}
			// Return the unique script matching the regexp.  If there are more
			// than one we don't return a value since there is no good way to
			// decide which one to return.  Returning a "random" one, say the
			// first, would introduce nondeterminism (or something close to it)
			// because the order is the heap iteration order.
			if (result_count == 1) {
				return last_result;
			} else {
				return undefined;
			}
		} else {
			return %GetScript(func_or_script_name);
		}
	}

	// Returns the script source. If the parameter is a function the return value
	// is the script source for the script in which the function is defined. If the
	// parameter is a string the return value is the script for which the script
	// name has that string value.
	function scriptSource(func_or_script_name) {
		return this.findScript(func_or_script_name).source;
	};
	
	function source(f) {
		if (!IS_FUNCTION(f)) throw new Error('Parameters have wrong types.');
		return %FunctionGetSourceCode(f);
	};
	
	function disassemble(f) {
		if (!IS_FUNCTION(f)) throw new Error('Parameters have wrong types.');
		return %DebugDisassembleFunction(f);
	};

	function disassembleConstructor(f) {
		if (!IS_FUNCTION(f)) throw new Error('Parameters have wrong types.');
		return %DebugDisassembleConstructor(f);
	};

	function ExecuteInDebugContext(f, without_debugger) {
		if (!IS_FUNCTION(f)) throw new Error('Parameters have wrong types.');
		return %ExecuteInDebugContext(f, !!without_debugger);
	};

	function sourcePosition(f) {
		if (!IS_FUNCTION(f)) throw new Error('Parameters have wrong types.');
		return %FunctionGetScriptSourcePosition(f);
	};

	function findFunctionSourceLocation(func, opt_line, opt_column) {
		local script = %FunctionGetScript(func);
		local script_offset = %FunctionGetScriptSourcePosition(func);
		return script.locationFromLine(opt_line, opt_column, script_offset);
	}

	// Returns the character position in a script based on a line number and an
	// optional position within that line.
	function findScriptSourcePosition(script, opt_line, opt_column) {
		local location = script.locationFromLine(opt_line, opt_column);
		return location ? location.position : null;
	}

	function findBreakPoint(break_point_number, remove) {
		local break_point;
		for (local i = 0; i < break_points.len(); i++) {
			if (break_points[i].number() == break_point_number) {
				break_point = break_points[i];
				// Remove the break point from the list if requested.
				if (remove) {
					break_points.splice(i, 1);
				}
				break;
			}
		}
		if (break_point) {
			return break_point;
		} else {
			return this.findScriptBreakPoint(break_point_number, remove);
		}
	};

	function findBreakPointActualLocations(break_point_number) {
		for (local i = 0; i < script_break_points.len(); i++) {
			if (script_break_points[i].number() == break_point_number) {
				return script_break_points[i].actual_locations();
			}
		}
		for (local i = 0; i < break_points.len(); i++) {
			if (break_points[i].number() == break_point_number) {
				return [break_points[i].actual_location];
			}
		}
		return [];
	}

	function setBreakPoint(func, opt_line, opt_column, opt_condition) {
		if (!IS_FUNCTION(func)) throw new Error('Parameters have wrong types.');
		// Break points in API functions are not supported.
		if (%FunctionIsAPIFunction(func)) {
			throw new Error('Cannot set break point in native code.');
		}
		// Find source position relative to start of the function
		local break_position =
			this.findFunctionSourceLocation(func, opt_line, opt_column).position;
		local source_position = break_position - this.sourcePosition(func);
		// Find the script for the function.
		local script = %FunctionGetScript(func);
		// Break in builtin JavaScript code is not supported.
		if (script.type == DebugScriptType.Native) {
			throw new Error('Cannot set break point in native code.');
		}
		// If the script for the function has a name convert this to a script break
		// point.
		if (script && script.id) {
			// Adjust the source position to be script relative.
			source_position += %FunctionGetScriptSourcePosition(func);
			// Find line and column for the position in the script and set a script
			// break point from that.
			local location = script.locationFromPosition(source_position, false);
			return this.setScriptBreakPointById(script.id,
												location.line, location.column,
												opt_condition);
		} else {
			// Set a break point directly on the function.
			local break_point = MakeBreakPoint(source_position, opt_line, opt_column);
			local actual_position =
				%SetFunctionBreakPoint(func, source_position, break_point);
			actual_position += this.sourcePosition(func);
			local actual_location = script.locationFromPosition(actual_position, true);
			break_point.actual_location = { line: actual_location.line,
			column: actual_location.column };
			break_point.setCondition(opt_condition);
			return break_point.number();
		}
	};

	function enableBreakPoint(break_point_number) {
		local break_point = this.findBreakPoint(break_point_number, false);
		break_point.enable();
	};

	function disableBreakPoint(break_point_number) {
		local break_point = this.findBreakPoint(break_point_number, false);
		break_point.disable();
	};

	function changeBreakPointCondition(break_point_number, condition) {
		local break_point = this.findBreakPoint(break_point_number, false);
		break_point.setCondition(condition);
	};

	function changeBreakPointIgnoreCount(break_point_number, ignoreCount) {
		if (ignoreCount < 0) {
			throw new Error('Invalid argument');
		}
		local break_point = this.findBreakPoint(break_point_number, false);
		break_point.setIgnoreCount(ignoreCount);
	};


	function clearBreakPoint(break_point_number) {
		local break_point = this.findBreakPoint(break_point_number, true);
		if (break_point) {
			return %ClearBreakPoint(break_point);
		} else {
			break_point = this.findScriptBreakPoint(break_point_number, true);
			if (!break_point) {
				throw new Error('Invalid breakpoint');
			}
		}
	};
	
	function clearAllBreakPoints() {
		for (local i = 0; i < break_points.len(); i++) {
			break_point = break_points[i];
			%ClearBreakPoint(break_point);
		}
		break_points = [];
	};

	function findScriptBreakPoint(break_point_number, remove) {
		local script_break_point;
		for (local i = 0; i < script_break_points.len(); i++) {
			if (script_break_points[i].number() == break_point_number) {
				script_break_point = script_break_points[i];
				// Remove the break point from the list if requested.
				if (remove) {
					script_break_point.clear();
					script_break_points.splice(i,1);
				}
				break;
			}
		}
		return script_break_point;
	}


	// Sets a breakpoint in a script identified through id or name at the
	// specified source line and column within that line.
	function setScriptBreakPoint(type, script_id_or_name,
								 opt_line, opt_column, opt_condition,
								 opt_groupId) {
		// Create script break point object.
		local script_break_point =
			new ScriptBreakPoint(type, script_id_or_name, opt_line, opt_column,
								 opt_groupId);
		
		script_break_point.setCondition(opt_condition);
		script_break_points.append(script_break_point);
		
		// Run through all scripts to see if this script break point matches any
		// loaded scripts.
		local scripts = this.scripts();
		for (local i = 0; i < scripts.len(); i++) {
			if (script_break_point.matchesScript(scripts[i])) {
				script_break_point.set(scripts[i]);
			}
		}
		
		return script_break_point.number();
	}


	function setScriptBreakPointById(script_id,
									 opt_line, opt_column,
									 opt_condition, opt_groupId) {
		return this.setScriptBreakPoint(DebugScriptBreakPointType.ScriptId,
										script_id, opt_line, opt_column,
										opt_condition, opt_groupId);
	}
	

	function setScriptBreakPointByName(script_name,
									   opt_line, opt_column,
									   opt_condition, opt_groupId) {
		return this.setScriptBreakPoint(DebugScriptBreakPointType.ScriptName,
										script_name, opt_line, opt_column,
										opt_condition, opt_groupId);
	}
	

	function enableScriptBreakPoint(break_point_number) {
		local script_break_point = this.findScriptBreakPoint(break_point_number, false);
		script_break_point.enable();
	};


	function disableScriptBreakPoint(break_point_number) {
		local script_break_point = this.findScriptBreakPoint(break_point_number, false);
		script_break_point.disable();
	};

	function changeScriptBreakPointCondition(break_point_number, condition) {
		local script_break_point = this.findScriptBreakPoint(break_point_number, false);
		script_break_point.setCondition(condition);
	};

	function changeScriptBreakPointIgnoreCount(break_point_number, ignoreCount) {
		if (ignoreCount < 0) {
			throw new Error('Invalid argument');
		}
		local script_break_point = this.findScriptBreakPoint(break_point_number, false);
		script_break_point.setIgnoreCount(ignoreCount);
	};
	

	function scriptBreakPoints() {
		return script_break_points;
	}


	function clearStepping() {
		%ClearStepping();
	}

	function setBreakOnException() {
		return %ChangeBreakOnException(DebugExceptionBreak.All, true);
	};

	function clearBreakOnException() {
		return %ChangeBreakOnException(DebugExceptionBreak.All, false);
	};

	function setBreakOnUncaughtException() {
		return %ChangeBreakOnException(DebugExceptionBreak.Uncaught, true);
	};

	function clearBreakOnUncaughtException() {
		return %ChangeBreakOnException(DebugExceptionBreak.Uncaught, false);
	};

	function showBreakPoints(f, full) {
		if (!IS_FUNCTION(f)) throw new Error('Parameters have wrong types.');
		local source = full ? this.scriptSource(f) : this.source(f);
		local offset = full ? this.sourcePosition(f) : 0;
		local locations = this.breakLocations(f);
		if (!locations) return source;
		locations.sort(function(x, y) { return x - y; });
		local result = "";
		local prev_pos = 0;
		local pos;
		for (local i = 0; i < locations.len(); i++) {
			pos = locations[i] - offset;
			result += source.slice(prev_pos, pos);
			result += "[B" + i + "]";
			prev_pos = pos;
		}
		pos = source.len();
		result += source.substring(prev_pos, pos);
		return result;
	};
	

	// Get all the scripts currently loaded. Locating all the scripts is based on
	// scanning the heap.
	function scripts() {
		// Collect all scripts in the heap.
		return %DebugGetLoadedScripts();
	}
}

function MakeExecutionState(break_id) {
	return new ExecutionState(break_id);
}

class ExecutionState {

	constructor(break_id) {
		this.break_id = break_id;
		this.selected_frame = 0;
	}

	function prepareStep(opt_action, opt_count) {
		local action = DebugStepAction.StepIn;
		if (!IS_UNDEFINED(opt_action)) action = %ToNumber(opt_action);
		local count = opt_count ? %ToNumber(opt_count) : 1;
		
		return %PrepareStep(this.break_id, action, count);
	}
	
	function evaluateGlobal(source, disable_break) {
		return MakeMirror(
			%DebugEvaluateGlobal(this.break_id, source, Boolean(disable_break)));
	};
	
	function frameCount() {
		return %GetFrameCount(this.break_id);
	};
	
	function threadCount() {
		return %GetThreadCount(this.break_id);
	};
	
	function frame(opt_index) {
		// If no index supplied return the selected frame.
		if (opt_index == null) opt_index = this.selected_frame;
		if (opt_index < 0 || opt_index >= this.frameCount())
			throw new Error('Illegal frame index.');
		return new FrameMirror(this.break_id, opt_index);
	};
	
	function cframesValue(opt_from_index, opt_to_index) {
		return %GetCFrames(this.break_id);
	};
	
	function setSelectedFrame(index) {
		local i = %ToNumber(index);
		if (i < 0 || i >= this.frameCount()) throw new Error('Illegal frame index.');
		this.selected_frame = i;
	};
	
	function selectedFrame() {
		return this.selected_frame;
	};
	
	function debugCommandProcessor(opt_is_running) {
		return new DebugCommandProcessor(this, opt_is_running);
	};
}

function MakeBreakEvent(exec_state, break_points_hit) {
	return new BreakEvent(exec_state, break_points_hit);
}


class BreakEvent {
	constructor(exec_state, break_points_hit) {
		this.exec_state_ = exec_state;
		this.break_points_hit_ = break_points_hit;
	}

	function executionState() {
		return this.exec_state_;
	};
	
	
	function eventType() {
		return DebugEvent.Break;
	};
	
	
	function func() {
		return this.exec_state_.frame(0).func();
	};
	
	
	function sourceLine() {
		return this.exec_state_.frame(0).sourceLine();
	};
	
	
	function sourceColumn() {
		return this.exec_state_.frame(0).sourceColumn();
	};
	
	
	function sourceLineText() {
		return this.exec_state_.frame(0).sourceLineText();
	};
	
	
	function breakPointsHit() {
		return this.break_points_hit_;
	};
	
	
	function toJSONProtocol() {
		local o = { seq: next_response_seq++,
		type: "event",
		event: "break",
		body: { invocationText: this.exec_state_.frame(0).invocationText(),
		}
		};
		
		// Add script related information to the event if available.
		local script = this.func().script();
		if (script) {
			o.body.sourceLine = this.sourceLine(),
			o.body.sourceColumn = this.sourceColumn(),
			o.body.sourceLineText = this.sourceLineText(),
			o.body.script = MakeScriptObject_(script, false);
		}
		
		// Add an Array of break points hit if any.
		if (this.breakPointsHit()) {
			o.body.breakpoints = [];
			for (local i = 0; i < this.breakPointsHit().len(); i++) {
				// Find the break point number. For break points originating from a
				// script break point supply the script break point number.
				local breakpoint = this.breakPointsHit()[i];
				local script_break_point = breakpoint.script_break_point();
				local number;
				if (script_break_point) {
					number = script_break_point.number();
				} else {
					number = breakpoint.number();
				}
				o.body.breakpoints.append(number);
			}
		}
		return JSON.stringify(ObjectToProtocolObject_(o));
	};
}

function MakeExceptionEvent(exec_state, exception, uncaught) {
  return new ExceptionEvent(exec_state, exception, uncaught);
}


class ExceptionEvent {
	
	constructor(exec_state, exception, uncaught) {
		this.exec_state_ = exec_state;
		this.exception_ = exception;
		this.uncaught_ = uncaught;
	}

	function executionState() {
		return this.exec_state_;
	};
	
	
	function eventType() {
		return DebugEvent.Exception;
	};
	
	
	function exception() {
		return this.exception_;
	}
	
	
	function uncaught() {
		return this.uncaught_;
	}
	
	
	function func() {
		return this.exec_state_.frame(0).func();
	};
	
	
	function sourceLine() {
		return this.exec_state_.frame(0).sourceLine();
	};

	function sourceColumn() {
		return this.exec_state_.frame(0).sourceColumn();
	};
	
	function sourceLineText() {
		return this.exec_state_.frame(0).sourceLineText();
	};
	
	function toJSONProtocol() {
		local o = new ProtocolMessage();
		o.event = "exception";
		o.body = { uncaught: this.uncaught_,
		exception: MakeMirror(this.exception_)
		};
		
		// Exceptions might happen whithout any JavaScript frames.
		if (this.exec_state_.frameCount() > 0) {
			o.body.sourceLine = this.sourceLine();
			o.body.sourceColumn = this.sourceColumn();
			o.body.sourceLineText = this.sourceLineText();
			
			// Add script information to the event if available.
			local script = this.func().script();
			if (script) {
				o.body.script = MakeScriptObject_(script, false);
			}
		} else {
			o.body.sourceLine = -1;
		}
		return o.toJSONProtocol();
	};
}

function MakeCompileEvent(exec_state, script, before) {
  return new CompileEvent(exec_state, script, before);
}


class CompileEvent {
	constructor(exec_state, script, before) {
		this.exec_state_ = exec_state;
		this.script_ = MakeMirror(script);
		this.before_ = before;
	}
	
	function executionState() {
		return this.exec_state_;
	};
	
	
	function eventType() {
		if (this.before_) {
			return DebugEvent.BeforeCompile;
		} else {
			return DebugEvent.AfterCompile;
		}
	};
	
	
	function script() {
		return this.script_;
	};
	
	
	function toJSONProtocol() {
		local o = new ProtocolMessage();
		o.running = true;
		if (this.before_) {
			o.event = "beforeCompile";
		} else {
			o.event = "afterCompile";
		}
		o.body = {};
		o.body.script = this.script_;
		
		return o.toJSONProtocol();
	}
}

function MakeNewFunctionEvent(func) {
  return new NewFunctionEvent(func);
}

class NewFunctionEvent {
	constructor(func) {
		this.func = func;
	}

	function eventType() {
		return DebugEvent.NewFunction;
	};

	function name() {
		return this.func.name;
	};

	function setBreakPoint(p) {
		Debug.setBreakPoint(this.func, p || 0);
	};
}


function MakeScriptCollectedEvent(exec_state, id) {
  return new ScriptCollectedEvent(exec_state, id);
}


class ScriptCollectedEvent {

	constructor(exec_state, id) {
		this.exec_state_ = exec_state;
		this.id_ = id;
	}

	function id() {
		return this.id_;
	};

	function executionState() {
		return this.exec_state_;
	};

	function toJSONProtocol() {
		local o = new ProtocolMessage();
		o.running = true;
		o.event = "scriptCollected";
		o.body = {};
		o.body.script = { id: this.id() };
		return o.toJSONProtocol();
	}
}


function MakeScriptObject_(script, include_source)
{
	local o = { id: script.id(),
	name: script.name(),
	lineOffset: script.lineOffset(),
	columnOffset: script.columnOffset(),
	lineCount: script.lineCount(),
	};
	if (!IS_UNDEFINED(script.data())) {
		o.data = script.data();
	}
	if (include_source) {
		o.source = script.source();
	}
	return o;
};

class ProtocolMessage {

	constructor(request) {
		// Update sequence number.
		this.seq = next_response_seq++;
		if (request) {
			// If message is based on a request this is a response. Fill the initial
			// response from the request.
			this.type = 'response';
			this.request_seq = request.seq;
			this.command = request.command;
		} else {
			// If message is not based on a request it is a dabugger generated event.
			this.type = 'event';
		}
		this.success = true;
		// Handler may set this field to control debugger state.
		this.running = undefined;
	}


	function setOption(name, value) {
		if (!this.options_) {
			this.options_ = {};
		}
		this.options_[name] = value;
	}


	function failed(message) {
		this.success = false;
		this.message = message;
	}

	function toJSONProtocol() {
		// Encode the protocol header.
		local json = {};
		json.seq= this.seq;
		if (this.request_seq) {
			json.request_seq = this.request_seq;
		}
		json.type = this.type;
		if (this.event) {
			json.event = this.event;
		}
		if (this.command) {
			json.command = this.command;
		}
		if (this.success) {
			json.success = this.success;
		} else {
			json.success = false;
		}
		if (this.body) {
			// Encode the body part.
			local bodyJson;
			local serializer = MakeMirrorSerializer(true, this.options_);
			if (this.body instanceof Mirror) {
				bodyJson = serializer.serializeValue(this.body);
			} else if (this.body instanceof Array) {
				bodyJson = [];
				for (local i = 0; i < this.body.len(); i++) {
					if (this.body[i] instanceof Mirror) {
						bodyJson.append(serializer.serializeValue(this.body[i]));
					} else {
						bodyJson.append(ObjectToProtocolObject_(this.body[i], serializer));
					}
				}
			} else {
				bodyJson = ObjectToProtocolObject_(this.body, serializer);
			}
			json.body = bodyJson;
			json.refs = serializer.serializeReferencedObjects();
		}
		if (this.message) {
			json.message = this.message;
		}
		json.running = this.running;
		return JSON.stringify(json);
	}
}

class DebugCommandProcessor {

	constructor(exec_state, opt_is_running) {
		this.exec_state_ = exec_state;
		this.running_ = opt_is_running || false;
	};

	function processDebugRequest (request) {
		return this.processDebugJSONRequest(request);
	}

	function createResponse(request) {
		return new ProtocolMessage(request);
	};

	
	function processDebugJSONRequest(json_request) {
		local request;  // Current request.
		local response;  // Generated response.
		try {
			try {
				// Convert the JSON string to an object.
				request = %CompileString('(' + json_request + ')', false)();
				
				// Create an initial response.
				response = this.createResponse(request);
				
				if (!request.type) {
					throw new Error('Type not specified');
				}
				
				if (request.type != 'request') {
					throw new Error("Illegal type '" + request.type + "' in request");
				}
				
				if (!request.command) {
					throw new Error('Command not specified');
				}
				
				if (request.arguments) {
					local args = request.arguments;
					// TODO(yurys): remove request.arguments.compactFormat check once
					// ChromeDevTools are switched to 'inlineRefs'
					if (args.inlineRefs || args.compactFormat) {
						response.setOption('inlineRefs', true);
					}
					if (!IS_UNDEFINED(args.maxStringLength)) {
						response.setOption('maxStringLength', args.maxStringLength);
					}
				}
				
				if (request.command == 'continue') {
					this.continueRequest_(request, response);
				} else if (request.command == 'break') {
					this.breakRequest_(request, response);
				} else if (request.command == 'setbreakpoint') {
					this.setBreakPointRequest_(request, response);
				} else if (request.command == 'changebreakpoint') {
					this.changeBreakPointRequest_(request, response);
				} else if (request.command == 'clearbreakpoint') {
					this.clearBreakPointRequest_(request, response);
				} else if (request.command == 'clearbreakpointgroup') {
					this.clearBreakPointGroupRequest_(request, response);
				} else if (request.command == 'listbreakpoints') {
					this.listBreakpointsRequest_(request, response);
				} else if (request.command == 'backtrace') {
					this.backtraceRequest_(request, response);
				} else if (request.command == 'frame') {
					this.frameRequest_(request, response);
				} else if (request.command == 'scopes') {
					this.scopesRequest_(request, response);
				} else if (request.command == 'scope') {
					this.scopeRequest_(request, response);
				} else if (request.command == 'evaluate') {
					this.evaluateRequest_(request, response);
				} else if (request.command == 'lookup') {
					this.lookupRequest_(request, response);
				} else if (request.command == 'references') {
					this.referencesRequest_(request, response);
				} else if (request.command == 'source') {
					this.sourceRequest_(request, response);
				} else if (request.command == 'scripts') {
					this.scriptsRequest_(request, response);
				} else if (request.command == 'threads') {
					this.threadsRequest_(request, response);
				} else if (request.command == 'suspend') {
					this.suspendRequest_(request, response);
				} else if (request.command == 'version') {
					this.versionRequest_(request, response);
				} else if (request.command == 'profile') {
					this.profileRequest_(request, response);
				} else if (request.command == 'changelive') {
					this.changeLiveRequest_(request, response);
				} else {
					throw new Error('Unknown command "' + request.command + '" in request');
				}
			} catch (e) {
				// If there is no response object created one (without command).
				if (!response) {
					response = this.createResponse();
				}
				response.success = false;
				response.message = %ToString(e);
			}
			
			// Return the response as a JSON encoded string.
			try {
				if (!IS_UNDEFINED(response.running)) {
					// Response controls running state.
					this.running_ = response.running;
				}
				response.running = this.running_;
				return response.toJSONProtocol();
			} catch (e) {
				// Failed to generate response - return generic error.
				return '{"seq":' + response.seq + ',' +
					'"request_seq":' + request.seq + ',' +
						'"type":"response",' +
							'"success":false,' +
								'"message":"Internal error: ' + %ToString(e) + '"}';
			}
		} catch (e) {
			// Failed in one of the catch blocks above - most generic error.
			return '{"seq":0,"type":"response","success":false,"message":"Internal error"}';
		}
	};
	
	
	function continueRequest_(request, response) {
		// Check for arguments for continue.
		if (request.arguments) {
			local count = 1;
			local action = DebugStepAction.StepIn;
			
			// Pull out arguments.
			local stepaction = request.arguments.stepaction;
			local stepcount = request.arguments.stepcount;
			
			// Get the stepcount argument if any.
			if (stepcount) {
				count = %ToNumber(stepcount);
				if (count < 0) {
					throw new Error('Invalid stepcount argument "' + stepcount + '".');
				}
			}
			
			// Get the stepaction argument.
			if (stepaction) {
				if (stepaction == 'in') {
					action = DebugStepAction.StepIn;
				} else if (stepaction == 'min') {
					action = DebugStepAction.StepMin;
				} else if (stepaction == 'next') {
					action = DebugStepAction.StepNext;
				} else if (stepaction == 'out') {
					action = DebugStepAction.StepOut;
				} else {
					throw new Error('Invalid stepaction argument "' + stepaction + '".');
				}
			}
			
			// Setup the VM for stepping.
			this.exec_state_.prepareStep(action, count);
		}
		
		// VM should be running after executing this request.
		response.running = true;
	};
	
	
	function breakRequest_(request, response) {
		// Ignore as break command does not do anything when broken.
	};
	
	
	function setBreakPointRequest_ =
		function(request, response) {
			// Check for legal request.
			if (!request.arguments) {
				response.failed('Missing arguments');
				return;
			}
			
			// Pull out arguments.
			local type = request.arguments.type;
			local target = request.arguments.target;
			local line = request.arguments.line;
			local column = request.arguments.column;
			local enabled = IS_UNDEFINED(request.arguments.enabled) ?
			true : request.arguments.enabled;
			local condition = request.arguments.condition;
			local ignoreCount = request.arguments.ignoreCount;
			local groupId = request.arguments.groupId;
			
			// Check for legal arguments.
			if (!type || IS_UNDEFINED(target)) {
				response.failed('Missing argument "type" or "target"');
				return;
			}
			if (type != 'function' && type != 'handle' &&
				type != 'script' && type != 'scriptId') {
				response.failed('Illegal type "' + type + '"');
				return;
			}
			
			// Either function or script break point.
			local break_point_number;
			if (type == 'function') {
				// Handle function break point.
				if (!IS_STRING(target)) {
					response.failed('Argument "target" is not a string value');
					return;
				}
				local f;
				try {
					// Find the function through a global evaluate.
					f = this.exec_state_.evaluateGlobal(target).value();
				} catch (e) {
					response.failed('Error: "' + %ToString(e) +
									'" evaluating "' + target + '"');
					return;
				}
				if (!IS_FUNCTION(f)) {
					response.failed('"' + target + '" does not evaluate to a function');
					return;
				}
				
				// Set function break point.
				break_point_number = Debug.setBreakPoint(f, line, column, condition);
			} else if (type == 'handle') {
				// Find the object pointed by the specified handle.
				local handle = parseInt(target, 10);
				local mirror = LookupMirror(handle);
				if (!mirror) {
					return response.failed('Object #' + handle + '# not found');
				}
				if (!mirror.isFunction()) {
					return response.failed('Object #' + handle + '# is not a function');
				}
				
				// Set function break point.
				break_point_number = Debug.setBreakPoint(mirror.value(),
														 line, column, condition);
			} else if (type == 'script') {
				// set script break point.
				break_point_number =
					Debug.setScriptBreakPointByName(target, line, column, condition,
													groupId);
			} else {  // type == 'scriptId.
				break_point_number =
					Debug.setScriptBreakPointById(target, line, column, condition, groupId);
			}
			
			// Set additional break point properties.
			local break_point = Debug.findBreakPoint(break_point_number);
			if (ignoreCount) {
				Debug.changeBreakPointIgnoreCount(break_point_number, ignoreCount);
			}
			if (!enabled) {
				Debug.disableBreakPoint(break_point_number);
			}
			
			// Add the break point number to the response.
			response.body = { type: type,
			breakpoint: break_point_number }
			
			// Add break point information to the response.
			if (break_point instanceof ScriptBreakPoint) {
				if (break_point.type() == DebugScriptBreakPointType.ScriptId) {
					response.body.type = 'scriptId';
					response.body.script_id = break_point.script_id();
				} else {
					response.body.type = 'scriptName';
					response.body.script_name = break_point.script_name();
				}
				response.body.line = break_point.line();
				response.body.column = break_point.column();
				response.body.actual_locations = break_point.actual_locations();
			} else {
				response.body.type = 'function';
				response.body.actual_locations = [break_point.actual_location];
			}
		};
	
	
	function changeBreakPointRequest_(request, response) {
		// Check for legal request.
		if (!request.arguments) {
			response.failed('Missing arguments');
			return;
		}
		
		// Pull out arguments.
		local break_point = %ToNumber(request.arguments.breakpoint);
		local enabled = request.arguments.enabled;
		local condition = request.arguments.condition;
		local ignoreCount = request.arguments.ignoreCount;
		
		// Check for legal arguments.
		if (!break_point) {
			response.failed('Missing argument "breakpoint"');
			return;
		}
		
		// Change enabled state if supplied.
		if (!IS_UNDEFINED(enabled)) {
			if (enabled) {
				Debug.enableBreakPoint(break_point);
			} else {
				Debug.disableBreakPoint(break_point);
			}
		}
		
		// Change condition if supplied
		if (!IS_UNDEFINED(condition)) {
			Debug.changeBreakPointCondition(break_point, condition);
		}
		
		// Change ignore count if supplied
		if (!IS_UNDEFINED(ignoreCount)) {
			Debug.changeBreakPointIgnoreCount(break_point, ignoreCount);
		}
	}
	
	
	function clearBreakPointGroupRequest_(request, response) {
		// Check for legal request.
		if (!request.arguments) {
			response.failed('Missing arguments');
			return;
		}
		
		// Pull out arguments.
		local group_id = request.arguments.groupId;
		
		// Check for legal arguments.
		if (!group_id) {
			response.failed('Missing argument "groupId"');
			return;
		}
		
		local cleared_break_points = [];
		local new_script_break_points = [];
		for (local i = 0; i < script_break_points.len(); i++) {
			local next_break_point = script_break_points[i];
			if (next_break_point.groupId() == group_id) {
				cleared_break_points.append(next_break_point.number());
				next_break_point.clear();
			} else {
				new_script_break_points.append(next_break_point);
			}
		}
		script_break_points = new_script_break_points;
		
		// Add the cleared break point numbers to the response.
		response.body = { breakpoints: cleared_break_points };
	}
	
	function clearBreakPointRequest_(request, response) {
		// Check for legal request.
		if (!request.arguments) {
			response.failed('Missing arguments');
			return;
		}

		// Pull out arguments.
		local break_point = %ToNumber(request.arguments.breakpoint);

		// Check for legal arguments.
		if (!break_point) {
			response.failed('Missing argument "breakpoint"');
			return;
		}

		// Clear break point.
		Debug.clearBreakPoint(break_point);
		
		// Add the cleared break point number to the response.
		response.body = { breakpoint: break_point }
	}
	
	function listBreakpointsRequest_(request, response) {
		local array = [];
		for (local i = 0; i < script_break_points.len(); i++) {
			local break_point = script_break_points[i];
			
			local description = {
			number: break_point.number(),
			line: break_point.line(),
			column: break_point.column(),
			groupId: break_point.groupId(),
				hit_count: break_point.hit_count(),
			active: break_point.active(),
			condition: break_point.condition(),
			ignoreCount: break_point.ignoreCount(),
				actual_locations: break_point.actual_locations()
				}
			
			if (break_point.type() == DebugScriptBreakPointType.ScriptId) {
				description.type = 'scriptId';
				description.script_id = break_point.script_id();
			} else {
				description.type = 'scriptName';
				description.script_name = break_point.script_name();
			}
			array.append(description);
		}
		
		response.body = { breakpoints: array }
	}
	
	
	function backtraceRequest_(request, response) {
		// Get the number of frames.
		local total_frames = this.exec_state_.frameCount();
		
		// Create simple response if there are no frames.
		if (total_frames == 0) {
			response.body = {
			totalFrames: total_frames
			}
			return;
		}
		
		// Default frame range to include in backtrace.
		local from_index = 0
			local to_index = kDefaultBacktraceLength;
		
		// Get the range from the arguments.
		if (request.arguments) {
			if (request.arguments.fromFrame) {
				from_index = request.arguments.fromFrame;
			}
			if (request.arguments.toFrame) {
				to_index = request.arguments.toFrame;
			}
			if (request.arguments.bottom) {
				local tmp_index = total_frames - from_index;
				from_index = total_frames - to_index
					to_index = tmp_index;
			}
			if (from_index < 0 || to_index < 0) {
				return response.failed('Invalid frame number');
			}
		}
		
		// Adjust the index.
		to_index = Math.min(total_frames, to_index);
		
		if (to_index <= from_index) {
			local error = 'Invalid frame range';
			return response.failed(error);
		}
		
		// Create the response body.
		local frames = [];
		for (local i = from_index; i < to_index; i++) {
			frames.append(this.exec_state_.frame(i));
		}
		response.body = {
		fromFrame: from_index,
		toFrame: to_index,
		totalFrames: total_frames,
		frames: frames
		}
	};


	function backtracec(cmd, args) {
		return this.exec_state_.cframesValue();
	};
	

	function frameRequest_(request, response) {
		// No frames no source.
		if (this.exec_state_.frameCount() == 0) {
			return response.failed('No frames');
		}
		
		// With no arguments just keep the selected frame.
		if (request.arguments) {
			local index = request.arguments.number;
			if (index < 0 || this.exec_state_.frameCount() <= index) {
				return response.failed('Invalid frame number');
			}
			
			this.exec_state_.setSelectedFrame(request.arguments.number);
		}
		response.body = this.exec_state_.frame();
	};


	function frameForScopeRequest_(request) {
		// Get the frame for which the scope or scopes are requested. With no frameNumber
		// argument use the currently selected frame.
		if (request.arguments && !IS_UNDEFINED(request.arguments.frameNumber)) {
			frame_index = request.arguments.frameNumber;
			if (frame_index < 0 || this.exec_state_.frameCount() <= frame_index) {
				return response.failed('Invalid frame number');
			}
			return this.exec_state_.frame(frame_index);
		} else {
			return this.exec_state_.frame();
		}
	}

	function scopesRequest_(request, response) {
		// No frames no scopes.
		if (this.exec_state_.frameCount() == 0) {
			return response.failed('No scopes');
		}
		
		// Get the frame for which the scopes are requested.
		local frame = this.frameForScopeRequest_(request);
		
		// Fill all scopes for this frame.
		local total_scopes = frame.scopeCount();
		local scopes = [];
		for (local i = 0; i < total_scopes; i++) {
			scopes.append(frame.scope(i));
		}
		response.body = {
		fromScope: 0,
		toScope: total_scopes,
		totalScopes: total_scopes,
		scopes: scopes
		}
	};
	
	
	function scopeRequest_(request, response) {
		// No frames no scopes.
		if (this.exec_state_.frameCount() == 0) {
			return response.failed('No scopes');
		}
		
		// Get the frame for which the scope is requested.
		local frame = this.frameForScopeRequest_(request);
		
		// With no scope argument just return top scope.
		local scope_index = 0;
		if (request.arguments && !IS_UNDEFINED(request.arguments.number)) {
			scope_index = %ToNumber(request.arguments.number);
			if (scope_index < 0 || frame.scopeCount() <= scope_index) {
				return response.failed('Invalid scope number');
			}
		}
		
		response.body = frame.scope(scope_index);
	};
	
	
	function evaluateRequest_(request, response) {
		if (!request.arguments) {
			return response.failed('Missing arguments');
		}

		// Pull out arguments.
		local expression = request.arguments.expression;
		local frame = request.arguments.frame;
		local global = request.arguments.global;
		local disable_break = request.arguments.disable_break;
		
		// The expression argument could be an integer so we convert it to a
		// string.
		try {
			expression = String(expression);
		} catch(e) {
			return response.failed('Failed to convert expression argument to string');
		}
		
		// Check for legal arguments.
		if (!IS_UNDEFINED(frame) && global) {
			return response.failed('Arguments "frame" and "global" are exclusive');
		}
		
		// Global evaluate.
		if (global) {
			// Evaluate in the global context.
			response.body =
				this.exec_state_.evaluateGlobal(expression, Boolean(disable_break));
			return;
		}
		
		// Default value for disable_break is true.
		if (IS_UNDEFINED(disable_break)) {
			disable_break = true;
		}
		
		// No frames no evaluate in frame.
		if (this.exec_state_.frameCount() == 0) {
			return response.failed('No frames');
		}
		
		// Check whether a frame was specified.
		if (!IS_UNDEFINED(frame)) {
			local frame_number = %ToNumber(frame);
			if (frame_number < 0 || frame_number >= this.exec_state_.frameCount()) {
				return response.failed('Invalid frame "' + frame + '"');
			}
			// Evaluate in the specified frame.
			response.body = this.exec_state_.frame(frame_number).evaluate(
				expression, Boolean(disable_break));
			return;
		} else {
			// Evaluate in the selected frame.
			response.body = this.exec_state_.frame().evaluate(
				expression, Boolean(disable_break));
			return;
		}
	};
	

	function lookupRequest_(request, response) {
		if (!request.arguments) {
			return response.failed('Missing arguments');
		}
		
		// Pull out arguments.
		local handles = request.arguments.handles;
		
		// Check for legal arguments.
		if (IS_UNDEFINED(handles)) {
			return response.failed('Argument "handles" missing');
		}
		
		// Set 'includeSource' option for script lookup.
		if (!IS_UNDEFINED(request.arguments.includeSource)) {
			includeSource = %ToBoolean(request.arguments.includeSource);
			response.setOption('includeSource', includeSource);
		}
		
		// Lookup handles.
		local mirrors = {};
		for (local i = 0; i < handles.len(); i++) {
			local handle = handles[i];
			local mirror = LookupMirror(handle);
			if (!mirror) {
				return response.failed('Object #' + handle + '# not found');
			}
			mirrors[handle] = mirror;
		}
		response.body = mirrors;
	};
	
	
	function referencesRequest_(request, response) {
		if (!request.arguments) {
			return response.failed('Missing arguments');
		}
		
		// Pull out arguments.
		local type = request.arguments.type;
		local handle = request.arguments.handle;
		
		// Check for legal arguments.
		if (IS_UNDEFINED(type)) {
			return response.failed('Argument "type" missing');
		}
		if (IS_UNDEFINED(handle)) {
			return response.failed('Argument "handle" missing');
		}
		if (type != 'referencedBy' && type != 'constructedBy') {
			return response.failed('Invalid type "' + type + '"');
		}
		
		// Lookup handle and return objects with references the object.
		local mirror = LookupMirror(handle);
		if (mirror) {
			if (type == 'referencedBy') {
				response.body = mirror.referencedBy();
			} else {
				response.body = mirror.constructedBy();
			}
		} else {
			return response.failed('Object #' + handle + '# not found');
		}
	};
	
	function sourceRequest_(request, response) {
		// No frames no source.
		if (this.exec_state_.frameCount() == 0) {
			return response.failed('No source');
		}
		
		local from_line;
		local to_line;
		local frame = this.exec_state_.frame();
		if (request.arguments) {
			// Pull out arguments.
			from_line = request.arguments.fromLine;
			to_line = request.arguments.toLine;
			
			if (!IS_UNDEFINED(request.arguments.frame)) {
				local frame_number = %ToNumber(request.arguments.frame);
				if (frame_number < 0 || frame_number >= this.exec_state_.frameCount()) {
					return response.failed('Invalid frame "' + frame + '"');
				}
				frame = this.exec_state_.frame(frame_number);
			}
		}
		
		// Get the script selected.
		local script = frame.func().script();
		if (!script) {
			return response.failed('No source');
		}
		
		// Get the source slice and fill it into the response.
		local slice = script.sourceSlice(from_line, to_line);
		if (!slice) {
			return response.failed('Invalid line interval');
		}
		response.body = {};
		response.body.source = slice.sourceText();
		response.body.fromLine = slice.from_line;
		response.body.toLine = slice.to_line;
		response.body.fromPosition = slice.from_position;
		response.body.toPosition = slice.to_position;
		response.body.totalLines = script.lineCount();
	};
	

	function ScriptTypeFlag(type) {
		return (1 << type);
	}
	
	function scriptsRequest_(request, response) {
		local types = ScriptTypeFlag(DebugScriptType.Normal);
		local includeSource = false;
		local idsToInclude = null;
		if (request.arguments) {
			// Pull out arguments.
			if (!IS_UNDEFINED(request.arguments.types)) {
				types = %ToNumber(request.arguments.types);
				if (isNaN(types) || types < 0) {
					return response.failed('Invalid types "' + request.arguments.types + '"');
				}
			}
			
			if (!IS_UNDEFINED(request.arguments.includeSource)) {
				includeSource = %ToBoolean(request.arguments.includeSource);
				response.setOption('includeSource', includeSource);
			}
			
			if (IS_ARRAY(request.arguments.ids)) {
				idsToInclude = {};
				local ids = request.arguments.ids;
				for (local i = 0; i < ids.len(); i++) {
					idsToInclude[ids[i]] = true;
				}
			}
		}
		
		// Collect all scripts in the heap.
		local scripts = %DebugGetLoadedScripts();
		
		response.body = [];
		
		for (local i = 0; i < scripts.len(); i++) {
			if (idsToInclude && !idsToInclude[scripts[i].id]) {
				continue;
			}
			if (types & ScriptTypeFlag(scripts[i].type)) {
				response.body.append(MakeMirror(scripts[i]));
			}
		}
	};
	
	
	function threadsRequest_(request, response) {
		// Get the number of threads.
		local total_threads = this.exec_state_.threadCount();
		
		// Get information for all threads.
		local threads = [];
		for (local i = 0; i < total_threads; i++) {
			local details = %GetThreadDetails(this.exec_state_.break_id, i);
			local thread_info = { current: details[0],
			id: details[1]
			}
			threads.append(thread_info);
		}
		
		// Create the response body.
		response.body = {
		totalThreads: total_threads,
		threads: threads
		}
	};
	
	
	function suspendRequest_(request, response) {
		response.running = false;
	};
	
	
	function versionRequest_(request, response) {
		response.body = {
		V8Version: %GetV8Version()
		}
	};
	
	
	function profileRequest_(request, response) {
		if (!request.arguments) {
			return response.failed('Missing arguments');
		}
		local modules = parseInt(request.arguments.modules);
		if (isNaN(modules)) {
			return response.failed('Modules is not an integer');
		}
		local tag = parseInt(request.arguments.tag);
		if (isNaN(tag)) {
			tag = 0;
		}
		if (request.arguments.command == 'resume') {
			%ProfilerResume(modules, tag);
		} else if (request.arguments.command == 'pause') {
			%ProfilerPause(modules, tag);
		} else {
			return response.failed('Unknown command');
		}
		response.body = {};
	};
	
	
	function changeLiveRequest_(request, response) {
		if (!Debug.LiveEdit) {
			return response.failed('LiveEdit feature is not supported');
		}
		if (!request.arguments) {
			return response.failed('Missing arguments');
		}
		local script_id = request.arguments.script_id;
		local preview_only = !!request.arguments.preview_only;
		
		local scripts = %DebugGetLoadedScripts();
		
		local the_script = null;
		for (local i = 0; i < scripts.len(); i++) {
			if (scripts[i].id == script_id) {
				the_script = scripts[i];
			}
		}
		if (!the_script) {
			response.failed('Script not found');
			return;
		}
		
		local change_log = new Array();
		
		if (!IS_STRING(request.arguments.new_source)) {
			throw "new_source argument expected";
		}
		
		local new_source = request.arguments.new_source;
		
		local result_description = Debug.LiveEdit.SetScriptSource(the_script,
																new_source, preview_only, change_log);
		response.body = {change_log: change_log, result: result_description};
	};
	
	
	// Check whether the previously processed command caused the VM to become
	// running.
	function isRunning() {
		return this.running_;
	}
	
	
	function systemBreak(cmd, args) {
		return %SystemBreak();
	};
	
	
	function NumberToHex8Str(n) {
		local r = "";
		for (local i = 0; i < 8; ++i) {
			local c = hexCharArray[n & 0x0F];  // hexCharArray is defined in uri.js
			r = c + r;
			n = n >>> 4;
		}
		return r;
	};
	
	function formatCFrames(cframes_value) {
		local result = "";
		if (cframes_value == null || cframes_value.len() == 0) {
			result += "(stack empty)";
		} else {
			for (local i = 0; i < cframes_value.len(); ++i) {
				if (i != 0) result += "\n";
				result += this.formatCFrame(cframes_value[i]);
			}
		}
		return result;
	};

	function formatCFrame(cframe_value) {
		local result = "";
		result += "0x" + NumberToHex8Str(cframe_value.address);
		if (!IS_UNDEFINED(cframe_value.text)) {
			result += " " + cframe_value.text;
		}
		return result;
	}
};

/**
 * Convert an Object to its debugger protocol representation. The representation
 * may be serilized to a JSON object using JSON.stringify().
 * This implementation simply runs through all string property names, converts
 * each property value to a protocol value and adds the property to the result
 * object. For type "object" the function will be called recursively. Note that
 * circular structures will cause infinite recursion.
 * @param {Object} object The object to format as protocol object.
 * @param {MirrorSerializer} mirror_serializer The serializer to use if any
 *     mirror objects are encountered.
 * @return {Object} Protocol object value.
 */
function ObjectToProtocolObject_(object, mirror_serializer)
{
	local content = {};
	for (local key in object) {
		// Only consider string keys.
		if (typeof key == 'string') {
			// Format the value based on its type.
			local property_value_json = ValueToProtocolValue_(object[key],
															mirror_serializer);
			// Add the property if relevant.
			if (!IS_UNDEFINED(property_value_json)) {
				content[key] = property_value_json;
			}
		}
	}
	
	return content;
}


/**
 * Convert an array to its debugger protocol representation. It will convert
 * each array element to a protocol value.
 * @param {Array} array The array to format as protocol array.
 * @param {MirrorSerializer} mirror_serializer The serializer to use if any
 *     mirror objects are encountered.
 * @return {Array} Protocol array value.
 */
function ArrayToProtocolArray_(array, mirror_serializer)
{
	local json = [];
	for (local i = 0; i < array.len(); i++) {
		json.append(ValueToProtocolValue_(array[i], mirror_serializer));
	}
	return json;
}


/**
 * Convert a value to its debugger protocol representation.
 * @param {*} value The value to format as protocol value.
 * @param {MirrorSerializer} mirror_serializer The serializer to use if any
 *     mirror objects are encountered.
 * @return {*} Protocol value.
 */
function ValueToProtocolValue_(value, mirror_serializer)
{
	// Format the value based on its type.
	local json;
	switch (typeof value) {
	case 'object':
		if (value instanceof Mirror) {
			json = mirror_serializer.serializeValue(value);
		} else if (IS_ARRAY(value)){
			json = ArrayToProtocolArray_(value, mirror_serializer);
		} else {
			json = ObjectToProtocolObject_(value, mirror_serializer);
		}
		break;
		
	case 'boolean':
	case 'string':
	case 'number':
		json = value;
		break
			
		default:
		json = null;
	}
	return json;
}
