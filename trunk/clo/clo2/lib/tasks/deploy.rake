desc "Deploy basic application directories"
task :deploy => :environment do
  dirs = %w{ app lib test public config script }
  onserver = "webuser@complearn.org:/home/webuser/hosting/clo/rails"
   ['public'].each { |d| File.chmod(0777, d) }
  dirs.each do | dir|
    cmd = <<EOF
rsync -avz -e ssh "#{RAILS_ROOT}/#{dir}" "#{onserver}" --exclude ".svn"
EOF
    puts cmd
    `#{cmd}`
  end
  `ssh webuser@asiaquake.org /usr/local/bin/restartapache`
end
