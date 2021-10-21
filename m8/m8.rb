# ***************************************************************************************
# ***************************************************************************************
#
#		Name : 		m8.rb
#		Author :	Paul Robson (paul@robsons.org.uk)
#		Date : 		21st October 2021
#		Purpose :	M8 Compiler
#
# ***************************************************************************************
# ***************************************************************************************

# ***************************************************************************************
#
# 									Storage element
#
# ***************************************************************************************

class Storage
	def initialize(base_address = $8000)
		@base = base_address
		@data = []
	#
	def add(byte)
		@data.append(byte)
	end 
	#
	def length 
		@data.length 
	end
end

# ***************************************************************************************
#
#  									General Word Classes
#
# ***************************************************************************************

class Word 
	def initialize(name)
		@name = name.strip.upcase 
	end
	#
	def get_name
		@name 
	end 
end

class ValueWord < Word
	def initialize(name,value)
		super(name)
		@value = value
	end
end

# ***************************************************************************************
#
# 								 Compiler word classes
#
# ***************************************************************************************

class Compiler 
	def initialize(base = 0x8000)
		@storage = Storage.new(base)
		@dictionary = {} 
		load_library "m8_#{base.to_s(16)}.lib"
	end 
	#
	def load_library(library_file)
		library = open()
	end
end



Compiler.new