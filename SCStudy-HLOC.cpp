//Sierra Chart Study 

#include "sierrachart.h"

SCDLLName("Previous Day Close Line Study")

SCSFExport scsf_PreviousDayCloseLine(SCStudyInterfaceRef sc)
{
    SCInputRef SessionEndTimeInput = sc.Input[0];

    if (sc.SetDefaults)
    {
        // Set the configuration and defaults
        sc.GraphName = "Previous Day Close Line";
        sc.StudyDescription = "Draws a horizontal line at the previous day's close price";

        // Set the study region and draw style
        sc.AutoLoop = 1;  // Enable AutoLoop to handle each bar automatically
        sc.GraphRegion = 0;

        // Inputs
        SessionEndTimeInput.Name = "Session End Time";
        SessionEndTimeInput.SetTime(HMS_TIME(22, 0, 0)); // Default to 22:00

        // Output subgraph for the line
        sc.Subgraph[0].Name = "Previous Day Close Line";
        sc.Subgraph[0].DrawStyle = DRAWSTYLE_STAIR_STEP;
        sc.Subgraph[0].PrimaryColor = RGB(255, 0, 0);
        sc.Subgraph[0].LineWidth = 2;
        sc.Subgraph[0].DrawZeros = false; // Do not draw the line where there is no valid data

        return;
    }

    // Variables to store the previous day's close price
    static float PreviousDayClosePrice = 0.0f;

    // Define the session times
    SCDateTime SessionStartTime = HMS_TIME(2, 15, 0); // 02:15
    SCDateTime SessionEndTime = HMS_TIME(21, 45, 0); // 21:45

    // Check if a new trading day has started
    if (sc.Index == 0 || sc.BaseDateTimeIn[sc.Index].GetDate() != sc.BaseDateTimeIn[sc.Index - 1].GetDate())
    {
        SCDateTime CurrentDate = sc.BaseDateTimeIn[sc.Index].GetDate();
        SCDateTime PreviousDate = sc.GetTradingDayStartDateTimeOfBar(sc.BaseDateTimeIn[sc.Index]).GetDate() - 1;

        // Adjust for weekends (if it's Monday, look back to Friday)
        int DayOfWeek = CurrentDate.GetDayOfWeek();
        if (DayOfWeek == MONDAY)
        {
            PreviousDate -= 2; // Go back to Friday
        }
        else if (DayOfWeek == SUNDAY)
        {
            PreviousDate -= 1; // If somehow trading happens on Sunday, use Saturday's date as previous
        }

        // Reset PreviousDayClosePrice before finding the value
        PreviousDayClosePrice = 0.0f;

        // Loop to find the previous day's close price at the end of the session
        for (int i = sc.ArraySize - 1; i >= 0; --i)
        {
            SCDateTime BarDateTime = sc.BaseDateTimeIn[i];
            if (BarDateTime.GetDate() == PreviousDate && BarDateTime.GetTime() == SessionEndTimeInput.GetTime())
            {
                PreviousDayClosePrice = sc.Close[i];
                break;
            }
        }
    }

    // Draw the line at the previous day's close price for the current trading day within the session times
    SCDateTime CurrentBarTime = sc.BaseDateTimeIn[sc.Index].GetTime();
    if (PreviousDayClosePrice != 0.0f && CurrentBarTime >= SessionStartTime && CurrentBarTime <= SessionEndTime)
    {
        sc.Subgraph[0][sc.Index] = PreviousDayClosePrice;
    }
    else
    {
        sc.Subgraph[0][sc.Index] = 0; // Ensure no line is drawn outside the session times
    }
}

