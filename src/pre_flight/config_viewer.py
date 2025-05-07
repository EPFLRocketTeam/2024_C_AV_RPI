
import argparse
import curses
import json
import os
import time
from typing import Dict, List

can_quit    = True
should_quit = False

interact_mode = False

manual = [
    "COMMANDS",
    "",
    " quit",
    " quit -f",
    " interact",
    " write",
    "",
    "INTERACT MODE",
    "",
    " ESC (Leave)",
    " ARROWS (Move)",
    " T (Toggle)",
    " E (Enable)",
    " D (Disable)",
    ""
]

def get_events (window: curses.window) -> List[int]:
    ch = window.getch()
    if ch == 0x1B:
        curses.halfdelay(1)
        next_ch = window.getch()
        curses.cbreak()

        if next_ch == ord('['):
            curses.halfdelay(1)
            third_ch = window.getch()
            curses.cbreak()
            if third_ch == ord('A'):
                return [ curses.KEY_UP ]
            elif third_ch == ord('B'):
                return [ curses.KEY_DOWN ]
            elif third_ch == ord('C'):
                return [ curses.KEY_RIGHT ]
            elif third_ch == ord('D'):
                return [ curses.KEY_LEFT ]
            elif third_ch == -1:
                return [ ch, next_ch ]
            else:
                return [ ch, next_ch, third_ch ]
        if next_ch == -1:
            return [ ch ]
        return [ ch, next_ch ]
    return [ ch ]

def check_cursor (cursor, array: List):
    length = len(array)
    if length == 0:
        return 0

    if cursor >= length:
        return cursor % length
    if cursor < 0:
        return (length - ((- cursor) % length)) % length
    return cursor

class Section:
    name  : str
    bind  : str = ""
    items : List[str]  = []
    vals  : List[bool] = []

    cursor: int = 0

    def get_name (self, name: str):
        return self.bind + name

    @staticmethod
    def create_section (name: str, items: List[str] = [], vals: List[bool] = []):
        section = Section()
        section.name = name
        section.items = items
        section.vals  = vals

        assert len(items) == len(vals)

        return section

class Interactor:
    sections: List[Section]
    cursor  : int = 0

    config : Dict[str, bool]
    config_path: str

    def set_window (self, window : curses.window):
        self.window = window

    def render_section (self):
        pass
    
    @property
    def working (self):
        return interact_mode
    def step (self):
        if not self.working:
            return
        
        for i, ch in enumerate(get_events(self.window)):
            if ch == 0x1B:
                global interact_mode

                interact_mode = False
                man.reset()
                man.render()
                break
            
            section = self.sections[self.cursor]
            if ch == ord('t') or ch == ord('T'):
                section.vals[section.cursor] = not section.vals[section.cursor]
            if ch == ord('e') or ch == ord('E'):
                section.vals[section.cursor] = True
            if ch == ord('d') or ch == ord('D'):
                section.vals[section.cursor] = False

            if ch == curses.KEY_UP:
                self.sections[self.cursor].cursor -= 1
            if ch == curses.KEY_DOWN:
                self.sections[self.cursor].cursor += 1
            if ch == curses.KEY_LEFT:
                self.cursor -= 1
            if ch == curses.KEY_RIGHT:
                self.cursor += 1

            self.render()
    def as_config (self):
        conf = { **self.config }
        for sec in self.sections:
            for key, val in zip(sec.items, sec.vals):
                conf[sec.get_name(key)] = val
        return conf
    def is_config_changed (self):
        c1 = self.config
        c2 = self.as_config()

        if set(c1.keys()) != set(c2.keys()):
            return True
        
        for key in c1.keys():
            if c1[key] != c2[key]:
                return True
        
        return False
    
    def render (self):
        self.cursor = check_cursor(self.cursor, self.sections)
        self.window.clear()

        rows, cols = self.window.getmaxyx()
        idx = 0
        lin = 0
        for sid, section in enumerate(self.sections):
            name = " " + section.name.upper() + " "

            if idx + len(name) > cols:
                idx = 0
                line += 1
            
            if sid == self.cursor:
                self.window.addstr(lin, idx, name, curses.color_pair(color_selected))
            else:
                self.window.addstr(lin, idx, name)
            idx += len(name)

        lin += 2
        section = self.sections[self.cursor]
        section.cursor = check_cursor(section.cursor, section.items)

        if len(section.items) != 0:
            max_name_size = max(map(len, section.items)) + 2

            for tid, target in enumerate(section.items):
                name = " " + target.upper() + " "
                name = name.ljust(max_name_size)
                if tid == section.cursor:
                    self.window.addstr(lin + tid, 1, name, curses.color_pair(color_selected))
                else:
                    self.window.addstr(lin + tid, 1, name)
                
                self.window.addstr(lin + tid, 3 + max_name_size, " ON ",  curses.color_pair(color_enabled  if     section.vals[tid] else color_placeholder))
                self.window.addstr(lin + tid, 7 + max_name_size, " OFF ", curses.color_pair(color_disabled if not section.vals[tid] else color_placeholder))
        else:
            self.window.addstr(lin, 1, "Configuration section is empty.")

        max_man_size = max(map(len, manual)) + 1
        for idx, man in enumerate(reversed(manual)):
            self.window.addstr( rows - 1 - idx, cols - max_man_size - 2, ' ', curses.color_pair(color_separator) )
            self.window.addstr( rows - 1 - idx, cols - max_man_size, man )
        self.window.addstr( rows - 1 - len(manual), cols - max_man_size - 2, ' ', curses.color_pair(color_separator) )
        self.window.addstr( rows - 2 - len(manual), cols - max_man_size - 2, ' ' * (max_man_size + 2), curses.color_pair(color_separator) )

        global can_quit
        can_quit = not self.is_config_changed()
        self.window.refresh()
    def write (self):
        self.config = self.as_config()
        global can_quit
        can_quit = True
        write_current_config(self.config_path, self.config)
                
class CommandManager:
    placeholder = "type a command"
    content     = ""

    def reset (self):
        self.placeholder = "type a command"
        self.content     = ""

    @property
    def working (self):
        return not interact_mode

    def set_window (self, window: curses.window):
        self.window = window
    
    def step (self):
        if not self.working:
            return
        
        for i, ch in enumerate(get_events(self.window)):
            if ch == curses.KEY_BACKSPACE or ch == 127 or ch == 8:
                self.content = self.content[:-1]
            elif ch == curses.KEY_ENTER or ch == 10 or ch == 13:
                self.run_command()
            elif 0 <= ch < 256 and str.isprintable( chr(ch) ):
                self.content += chr(ch)
            
            self.render()
    def render (self):
        self.window.clear()

        rows, cols = self.window.getmaxyx()
        def use_str (string: str, *args):
            if len(string) >= cols:
                string = "..." + string[- cols + 4:]
            self.window.addstr(0, 0, string.ljust(cols - 1), *args)

        if self.content == "":
            use_str(self.placeholder, curses.color_pair(color_placeholder))
        else:
            use_str(self.content)

        self.window.refresh()

    def run_command (self):
        global can_quit, should_quit, interact_mode

        if self.content == "quit":
            if can_quit:
                should_quit = True
            else:
                self.placeholder = "ERROR: cannot quit ('write' or 'quit -f')"
                self.content     = ""
        if self.content == "quit -f":
            should_quit = True
        if self.content == "write":
            int.write()
            self.placeholder = "Wrote config file."
            self.content = ""
        if self.content == "interact":
            self.placeholder = ":INTERACT MODE"
            self.content = ""
            interact_mode = True
        
        self.render()

man = CommandManager()
int = Interactor()

int.sections = []

def read_scheme (config: Dict[str, bool]):
    file_path = os.path.join(os.path.dirname(__file__), "config_scheme.json")
    with open(file_path, "r") as file:
        data = json.loads( file.read() )
    
    int.config = config

    for u in data:
        sec = Section()
        sec.bind = "" if u["type"] == "all" else u["name"] + "."
        sec.name = u["name"]

        sec.items = []
        sec.vals  = []

        for key in u["items"]:
            sec.items.append(key)
            sec.vals.append(config.get( sec.get_name(key), False ))
        
        int.sections.append(sec)
def read_current_config (config_path: str):
    int.config_path = config_path
    
    config = {}
    with open(config_path, "r") as file:
        section = ""
        for line in file.readlines():
            line = line.strip()
            if line.strip() == "":
                continue
            if len(line) >= 2 and line[0] == '[' and line[-1] == ']':
                section = line[1:-1] + '.'
            else:
                name, word = line.split('=')
                name = name.strip()
                word = word.strip()
                config[section + name] = word == "on"

    read_scheme(config)
def write_current_config (config_path: str, config: Dict[str, bool]):
    sections: Dict[str, List[str]] = { "": [] }
    for key in config.keys():
        sec, tarn, tarv = (key.split('.', 1)[0], key.split('.', 1)[1], key)  if '.' in key else ("", key, key)

        if sec not in sections:
            sections[sec] = []
        
        sections[sec].append((tarn, tarv))
    
    keys = list( sections.keys() )
    keys.sort()
    assert keys[0] == ""

    payload = []
    for key in keys:
        value = sections[key]
        if key != "":
            payload.append(f"[{key}]")
        for sky, var in value:
            tar = "on" if config[var] else "off"
            payload.append(f"{sky}={tar}")
        payload.append("")
    
    with open(config_path, "w") as file:
        file.write("\n".join(payload))

def init_colors ():
    count_pairs = 0
    def init_color_pair (a, b):
        nonlocal count_pairs
        count_pairs += 1
        curses.init_pair(count_pairs, a, b)
        return count_pairs

    global color_separator
    color_separator = init_color_pair( curses.COLOR_WHITE, 237 )

    global color_placeholder
    color_placeholder = init_color_pair( 246, curses.COLOR_BLACK )

    global color_selected
    color_selected = init_color_pair( curses.COLOR_WHITE, 237 )

    global color_type_off
    color_type_off = color_placeholder

    global color_enabled, color_disabled
    color_enabled  = init_color_pair(34, 22)
    color_disabled = init_color_pair(88, 160)

def render_full (stdscr: curses.window):
    rows, cols = stdscr.getmaxyx()

    stdscr.clear()

    stdscr.addstr(0, 0, f"can_change_color: {curses.can_change_color()}")
    stdscr.addstr(1, 0, f"has_colors: {curses.has_colors()}")
    stdscr.addstr(rows - 2, 0, " " * cols, curses.color_pair( color_separator ))

    man.set_window( stdscr.subwin(1, cols, rows - 1, 0) )
    man.render()

    int.set_window( stdscr.subwin(rows - 2, cols, 0, 0) )
    int.render()
    
    stdscr.refresh()

def main_display (stdscr: curses.window):
    stdscr.clear()
    stdscr.keypad(True)
    init_colors()

    render_full(stdscr)

    while not should_quit:
        man.step()
        int.step()

def main ():
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("config")
    args = parser.parse_args()

    read_current_config(args.config)

    curses.wrapper(main_display)
if __name__ == "__main__":
    main()
