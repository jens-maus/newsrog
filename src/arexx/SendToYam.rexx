/* -- procedure to send article via email using YAM ----------------------- */


/* Argument parsing in ARexx is brain-damaged */

parse arg '"' ReplyTo '"' ':=:' '"' Subj '"' ':=:' '"' FSpec '"' rest

/* Prepend Re: if needed */

if left(Subj,4) ~= 'Re: '
then Subj='Re: '||Subj

/* If YAM isn't running, try to launch it */

if pos('YAM', (show(ports))) = 0 then do
   address command 'run YAM:YAM'
   do while pos('YAM', (show(ports))) == 0
      address command 'wait 1 sec'
   end
end

/* Ship it off to YAM, again handling ARexx brain-damage */

address YAM

Show
MailWrite
WriteMailTo  '"'||ReplyTo||'"'
WriteSubject '"'||Subj||'"'
WriteLetter  '"'||FSpec||'"'

return 0
