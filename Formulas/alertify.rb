class Alertify < Formula
  desc "CLI tool for managing reminders"
  homepage "https://github.com/amoghrrathod/alertify"
  url "https://github.com/amoghrrathod/alertify/releases/download/v1.0.0/alertify-1.0.0.tar.gz"
  sha256 "1c8333d3ea2ca2c5214121e1f30d7b3e8b71c19b3f0edc5b51703e31211e06e9"

  depends_on "jansson"
  depends_on "uuid"

  def install
    system "make", "install", "PREFIX=#{prefix}"
  end

  test do
    system "#{bin}/alertify", "--version"
  end
end
