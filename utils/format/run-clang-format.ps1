$WorkspaceRootDir = Join-Path $PSScriptRoot ../..
$ClangFormatPath = Join-Path $WorkspaceRootDir tools/clang-format

function FormatDir {
    param($DirPath)

    $RelativePath = Get-Item $DirPath | Resolve-Path -Relative
	Write-host "Formatting: $RelativePath"

    Set-Location $DirPath
    if (Test-Path -Path *.cpp -PathType Leaf)
    {
        &$ClangFormatPath --style=file -i *.cpp
    }

    if (Test-Path -Path *.hpp -PathType Leaf)
    {
        &$ClangFormatPath --style=file -i *.hpp
    }

    if (Test-Path -Path *.inl -PathType Leaf)
    {
        &$ClangFormatPath --style=file -i *.inl
    }

    Set-Location $WorkspaceRootDir
}

function IterateDir {
    param($DirPath)

    Get-ChildItem -Path $DirPath -Directory |
        foreach {
            if (!($_ -match "^\.|third_party|build|docs|tools|premake|utils|assets")) {
                FormatDir($_.FullName)
                IterateDir($_.FullName)
            }
        }
}

IterateDir($WorkspaceRootDir)