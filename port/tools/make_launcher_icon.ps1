param(
    [Parameter(Mandatory=$true)][string]$Source,
    [Parameter(Mandatory=$true)][string]$Output
)
# Preserve the supplied PNG; emit standard icon sizes with nearest-neighbour scaling.
$ErrorActionPreference='Stop'
Add-Type -AssemblyName System.Drawing
$sourcePath=(Resolve-Path -LiteralPath $Source).Path
$outputPath=$ExecutionContext.SessionState.Path.GetUnresolvedProviderPathFromPSPath($Output)
$image=[Drawing.Bitmap]::FromFile($sourcePath)
$entries=New-Object System.Collections.Generic.List[object]
try {
    $maximum=[Math]::Max($image.Width,$image.Height)
    foreach($size in @(16,24,32,48,64,128,256)) {
        $bitmap=New-Object Drawing.Bitmap($size,$size,[Drawing.Imaging.PixelFormat]::Format32bppArgb)
        $graphics=[Drawing.Graphics]::FromImage($bitmap)
        $memory=New-Object IO.MemoryStream
        try {
            $graphics.Clear([Drawing.Color]::Transparent)
            $graphics.CompositingMode=[Drawing.Drawing2D.CompositingMode]::SourceCopy
            $graphics.InterpolationMode=[Drawing.Drawing2D.InterpolationMode]::NearestNeighbor
            $graphics.PixelOffsetMode=[Drawing.Drawing2D.PixelOffsetMode]::Half
            $width=[int][Math]::Round($size*$image.Width/[double]$maximum)
            $height=[int][Math]::Round($size*$image.Height/[double]$maximum)
            $rect=New-Object Drawing.Rectangle([int][Math]::Floor(($size-$width)/2.0),[int][Math]::Floor(($size-$height)/2.0),$width,$height)
            $graphics.DrawImage($image,$rect,0,0,$image.Width,$image.Height,[Drawing.GraphicsUnit]::Pixel)
            $bitmap.Save($memory,[Drawing.Imaging.ImageFormat]::Png)
            $entries.Add(@{Size=$size;Bytes=$memory.ToArray()})
        } finally { $memory.Dispose();$graphics.Dispose();$bitmap.Dispose() }
    }
    $file=[IO.File]::Create($outputPath)
    $writer=New-Object IO.BinaryWriter($file)
    try {
        $writer.Write([UInt16]0);$writer.Write([UInt16]1);$writer.Write([UInt16]$entries.Count)
        $offset=6+16*$entries.Count
        foreach($entry in $entries) {
            $dimension=if($entry.Size -eq 256){0}else{$entry.Size}
            $writer.Write([byte]$dimension);$writer.Write([byte]$dimension)
            $writer.Write([byte]0);$writer.Write([byte]0)
            $writer.Write([UInt16]1);$writer.Write([UInt16]32)
            $writer.Write([UInt32]$entry.Bytes.Length);$writer.Write([UInt32]$offset)
            $offset+=$entry.Bytes.Length
        }
        foreach($entry in $entries){$writer.Write([byte[]]$entry.Bytes)}
    } finally { $writer.Dispose();$file.Dispose() }
} finally { $image.Dispose() }
