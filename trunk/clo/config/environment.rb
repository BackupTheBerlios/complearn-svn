# Copyright (c) 2006 Rudi Cilibrasi, Rulers of the RHouse
# All rights reserved.     cilibrar@cilibrar.com
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#     # Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     # Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     # Neither the name of the RHouse nor the
#       names of its contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE RULERS AND CONTRIBUTORS "AS IS" AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE RULERS AND CONTRIBUTORS BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# Load the Rails framework and configure your application.
# You can include your own configuration at the end of this file.
#
# Be sure to restart your webserver when you modify this file.

# The path to the root directory of your application.
RAILS_ROOT = File.join(File.dirname(__FILE__), '..')

# The environment your application is currently running.  Don't set
# this here; put it in your webserver's configuration as the RAILS_ENV
# environment variable instead.
#
# See config/environments/*.rb for environment-specific configuration.
RAILS_ENV  = ENV['RAILS_ENV'] || 'development'


# Load the Rails framework.  Mock classes for testing come first.
ADDITIONAL_LOAD_PATHS = ["#{RAILS_ROOT}/test/mocks/#{RAILS_ENV}"]

# Then model subdirectories.
ADDITIONAL_LOAD_PATHS.concat(Dir["#{RAILS_ROOT}/app/models/[_a-z]*"])
ADDITIONAL_LOAD_PATHS.concat(Dir["#{RAILS_ROOT}/components/[_a-z]*"])

# Followed by the standard includes.
ADDITIONAL_LOAD_PATHS.concat %w(
  app 
  app/models 
  app/controllers 
  app/helpers 
  app/apis 
  components 
  config 
  lib 
  vendor 
  vendor/rails/railties
  vendor/rails/railties/lib
  vendor/rails/actionpack/lib
  vendor/rails/activesupport/lib
  vendor/rails/activerecord/lib
  vendor/rails/actionmailer/lib
  vendor/rails/actionwebservice/lib
).map { |dir| "#{RAILS_ROOT}/#{dir}" }.select { |dir| File.directory?(dir) }

# Prepend to $LOAD_PATH
ADDITIONAL_LOAD_PATHS.reverse.each { |dir| $:.unshift(dir) if File.directory?(dir) }

# Require Rails libraries.
require 'rubygems' unless File.directory?("#{RAILS_ROOT}/vendor/rails")

require 'active_support'
require 'active_record'
require 'action_controller'
require 'action_mailer'
require 'action_web_service'

# Environment-specific configuration.
require_dependency "environments/#{RAILS_ENV}"
ActiveRecord::Base.configurations = File.open("#{RAILS_ROOT}/config/database.yml") { |f| YAML::load(f) }
ActiveRecord::Base.establish_connection


# Configure defaults if the included environment did not.
begin
  RAILS_DEFAULT_LOGGER = Logger.new("#{RAILS_ROOT}/log/#{RAILS_ENV}.log")
  RAILS_DEFAULT_LOGGER.level = (RAILS_ENV == 'production' ? Logger::INFO : Logger::DEBUG)
rescue StandardError
  RAILS_DEFAULT_LOGGER = Logger.new(STDERR)
  RAILS_DEFAULT_LOGGER.level = Logger::WARN
  RAILS_DEFAULT_LOGGER.warn(
    "Rails Error: Unable to access log file. Please ensure that log/#{RAILS_ENV}.log exists and is chmod 0666. " +
    "The log level has been raised to WARN and the output directed to STDERR until the problem is fixed."
  )
end

[ActiveRecord, ActionController, ActionMailer].each { |mod| mod::Base.logger ||= RAILS_DEFAULT_LOGGER }
[ActionController, ActionMailer].each { |mod| mod::Base.template_root ||= "#{RAILS_ROOT}/app/views/" }

# Set up routes.
ActionController::Routing::Routes.reload

Controllers = Dependencies::LoadingModule.root(
  File.join(RAILS_ROOT, 'app', 'controllers'),
  File.join(RAILS_ROOT, 'components')
)

# Include your app's configuration here:

