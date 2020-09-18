const sleep = (milliseconds) => {
  return new Promise(resolve => setTimeout(resolve, milliseconds));
}

var execute_cmd_seq = function(action_script) {
    parseActionScript(action_script, true);
}


async function parseActionScript(action_script, execute = false) {
    if(action_script.trim().length==0)
        return false;

    action_script=action_script.replace(/[{]/g,'{,')
    action_script=action_script.replace(/[}]/g,',}')
    var cmd_sequence = action_script.split(',');
    var valid = true;

    var pc=0;
    var pc_loop_begin=[];
    var lc=[];
    var loop_depth=0;

    var joy_cmd_tokens=null;

    while (pc < cmd_sequence.length) {
        //alert(cmd);
        var cmd = cmd_sequence[pc];
        pc++;

        if(cmd.trim().match(/^loop[0-9]+[{]$/) != null)
        {
            if(execute)
            {
                loop_depth++;
                lc[loop_depth]=parseInt(cmd.match(/[0-9]+/));
                pc_loop_begin[loop_depth]=pc;
                //alert(lc);
            }
        }
        else if(cmd.trim().length == 1 && cmd.trim()=='}')
        {
            if(execute)
            {
                lc[loop_depth]--;
                if(lc[loop_depth]>0)
                {
                    pc=pc_loop_begin[loop_depth];
                }
                else
                {
                    loop_depth--;
                }

            }
           
        }
        else if(cmd.trim().length == 1)
        {
            if(execute)
            {            
                var c64code = translateKey(cmd, cmd.toLowerCase());
                if(c64code !== undefined){
                    wasm_key(c64code[0], c64code[1], 1);
                    setTimeout(function() {wasm_key(c64code[0], c64code[1], 0);}, 100);
                }
            }
        }
        else if(cmd == 'pause')
        {
            if(execute)
            {
                //if(is_running())
                {
                    wasm_pause();
                } 
            }
        
        }
        else if(cmd == 'run')
        {
            if(execute)
            {
                //if(is_running())
                {
                    wasm_run();
                } 
            }
        }
        else if(cmd.trim().match(/^[0-9]+ms$/) != null)
        {
            //alert('sleep '+parseInt(cmd.substring(5)));
            if(execute)
            {
                await sleep(parseInt(cmd.match(/[0-9]+/)));                 
            }
        }
        else if(
            (
                joy_cmd_tokens=cmd.trim().match(/^j([12])(fire|up|down|right|left)([01])$/)
            )
            != null
        )
        {
            //alert('sleep '+parseInt(cmd.substring(5)));
            if(execute)
            {
                execute_joystick_script(joy_cmd_tokens);
            }
        }
        else
        {
            valid = false;
            break;
        }
    }//);
    return valid;
};


async function validate_custom_key(){

    var is_valid=true;
    $('#input_button_text').removeClass("is-valid");
    $('#input_button_text').removeClass("is-invalid");
    if( $('#input_button_text').val().trim().length==0)
    {
        $('#input_button_text').addClass("is-invalid");
        is_valid=false;
    }
    else
    {  
        $('#input_button_text').addClass("is-valid");
    }

    $('#input_action_script').removeClass("is-valid");
    $('#input_action_script').removeClass("is-invalid");
    if( (await parseActionScript($('#input_action_script').val())) == false)
    {
        $('#input_action_script').addClass("is-invalid");
        is_valid=false;
    }
    else
    {
        $('#input_action_script').addClass("is-valid");
    }
    return is_valid;
};


function execute_joystick_script(cmd_tokens)
{
    var portnr=cmd_tokens[1];
    var dir= cmd_tokens[2].toUpperCase();
    var down_or_release = cmd_tokens[3];

    if(dir == "FIRE")
    {
        wasm_joystick(portnr+(down_or_release == 1 ?"PRESS_"+dir:"RELEASE_"+dir));
    }
    else
    {
        wasm_joystick(portnr+(down_or_release == 1 ?"PULL_"+dir:"RELEASE_"+((dir=="LEFT" || dir=="RIGHT")?"X":"Y")));
    }
 }