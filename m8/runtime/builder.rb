# ***************************************************************************************
# ***************************************************************************************
#
#		Name : 		builder.rb
#		Author :	Paul Robson (paul@robsons.org.uk)
#		Date : 		21st October 2021
#		Purpose :	Builds vocabulary
#
# ***************************************************************************************
# ***************************************************************************************

vocab = open("_built.asm","w")
open_label = nil
next_label = 1000
Dir.glob('./*.def').each do |f|
	open(f).collect { |a| a.rstrip.gsub("\t"," ") }.each do |l|
		if l[0..5] == "@calls" or l[0..6] == "@copies"
			vocab.write("end#{open_label}:\n") if open_label
			open_label = next_label
			next_label += 1
			vocab.write(" .dw end#{open_label}-$\n") if open_label
			vocab.write(" .db '#{l.split[1]}',0\n")
			n = l[0..6] == "@copies" ? "end#{open_label}-$" : "$FF"
			vocab.write(" .db #{n}\n")
			vocab.write("start#{open_label}:\n") if open_label
		else
			vocab.write(l+"\n") if l[0] != ';'
		end
	end	
end
vocab.write("end#{open_label}:\n") if open_label
vocab.write(" .dw 0\n\n")
vocab.close
