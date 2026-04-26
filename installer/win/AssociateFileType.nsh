!macro AssociateFileType FileType Name Icon ShellCommand ID PerceivedType
    WriteRegStr HKCR "${FileType}" "" "${ID}"
    WriteRegStr HKCR "${FileType}" "PerceivedType" "${PerceivedType}"

    WriteRegStr HKCR "${ID}" "" "${Name}"
    WriteRegStr HKCR "${ID}\DefaultIcon" "" '${Icon}'
    WriteRegStr HKCR "${ID}\shell" "" "open"
    WriteRegStr HKCR "${ID}\shell\open\command" "" '${ShellCommand}'
!macroend