!macro RegisterURIScheme SchemeName Description Icon ApplicationPath
    WriteRegStr HKCR "${SchemeName}" "" "URL:${Description}"
    WriteRegStr HKCR "${SchemeName}" "URL Protocol" ""
    WriteRegStr HKCR "${SchemeName}\DefaultIcon" "" "${Icon}"
    WriteRegStr HKCR "${SchemeName}\shell\open\command" "" '"${ApplicationPath}" "%1"'
!macroend

!macro UnregisterURIScheme SchemeName
    DeleteRegKey HKCR "${SchemeName}"
!macroend