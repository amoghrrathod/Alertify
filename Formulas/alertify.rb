class Alertify < Formula
  desc "Reminder management CLI tool"
  homepage "https://github.com/amoghrrathod/alertify"
  url "https://github.com/yourusername/alertify/archive/v1.0.tar.gz"
  sha256 "your_tarball_sha256"

  depends_on "jansson"
  depends_on "ossp-uuid" # macOS Homebrew package

  def install
    system "make"
    bin.install "alertify"
  end

  test do
    system "#{bin}/alertify", "--version"
  end
end
