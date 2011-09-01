/******************************************************************************
*                                                                             *
*   PROJECT : EOS Digital Software Development Kit EDSDK                      *
*      NAME : ImageQualityPopUp.m                                                   *
*                                                                             *
*   Description: This is the Sample code to show the usage of EDSDK.          *
*                                                                             *
*                                                                             *
*******************************************************************************
*                                                                             *
*   Written and developed by Camera Design Dept.53                            *
*   Copyright Canon Inc. 2007-2010 All Rights Reserved                        *
*                                                                             *
*******************************************************************************
*   File Update Information:                                                  *
*     DATE      Identify    Comment                                           *
*   -----------------------------------------------------------------------   *
*   06-03-22    F-001        create first version.                            *
*                                                                             *
******************************************************************************/

#import "ImageQualityPopUp.h"
#import "cameraEvent.h"
#import "CameraModel.h"
#import "EDSDK.h"

@interface ImageQualityPopUp (Local)
-(void)initializeData;
@end

@implementation ImageQualityPopUp
- (id)initWithCoder:(NSCoder *)decoder
{
	[super initWithCoder:decoder];
	
//	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(update:) name:VIEW_UPDATE_MESSAGE object:nil];
	[self initializeData];
	
	return self;
}

-(void)dealloc
{
	[super dealloc];
}

-(void)update:(NSNotification *)notification
{
	CameraEvent * event;
	CameraModel * model = [notification object];
	NSDictionary* dict = [notification userInfo];
	NSString * command;
	EdsPropertyDesc desc;
	NSNumber * number;
	
	event = [dict objectForKey:@"event"];
	if(event == nil)
	{
		return ;
	}

	command = [event getEvent];
	number = [event getArg];
	//Update property
	if([command isEqualToString:@"PropertyChanged"])
	{
		if([number intValue] == kEdsPropID_ImageQuality)
		{
			[self updateProperty:[model getPropertyUInt32:[number intValue]]];
		}
	}

	//Update of list that can set property
	if([command isEqualToString:@"PropertyDescChanged"])
	{
		if([number intValue] == kEdsPropID_ImageQuality)
		{
			desc = [model getPropertyDesc:[number intValue]];
			[self updatePropertyDesc:&desc];
		}
	}
}

@end

@implementation ImageQualityPopUp (Local)

-(void)initializeData
{
	[self removeAllItems];
	
	// PTP Camera
	_propertyList = [[ NSDictionary alloc] initWithObjectsAndKeys : 
			@"RAW" ,		[NSNumber numberWithInt:0x00640f0f] , 
			@"Middle RAW(Small RAW1)" ,	[NSNumber numberWithInt:0x01640f0f] ,
			@"Small RAW(Small RAW2)" ,	[NSNumber numberWithInt:0x02640f0f] ,
					 
			@"RAW + Large Fine Jpeg" ,		[NSNumber numberWithInt:0x00640013] , 
			@"RAW + Middle Fine Jpeg" ,		[NSNumber numberWithInt:0x00640113] , 
			@"RAW + Small Fine Jpeg" ,		[NSNumber numberWithInt:0x00640213] , 
			@"RAW + Large Normal Jpeg" ,	[NSNumber numberWithInt:0x00640012] , 
			@"RAW + Middle Normal Jpeg" ,	[NSNumber numberWithInt:0x00640112] , 
			@"RAW + Small Normal Jpeg" ,	[NSNumber numberWithInt:0x00640212] , 
					 
			@"Middle RAW(Small RAW1) + Large Fine Jpeg" ,	[NSNumber numberWithInt:0x01640013] , 
			@"Middle RAW(Small RAW1) + Middle Fine Jpeg" ,	[NSNumber numberWithInt:0x01640113] , 
			@"Middle RAW(Small RAW1) + Small Fine Jpeg" ,	[NSNumber numberWithInt:0x01640213] , 
			@"Middle RAW(Small RAW1) + Large Normal Jpeg" ,	[NSNumber numberWithInt:0x01640012] , 
			@"Middle RAW(Small RAW1) + Middle Normal Jpeg" , [NSNumber numberWithInt:0x01640112] , 
			@"Middle RAW(Small RAW1) + Small Normal Jpeg" ,	[NSNumber numberWithInt:0x01640212] , 
			
			@"Small RAW(Small RAW2) + Large Fine Jpeg" ,	[NSNumber numberWithInt:0x02640013] , 
			@"Small RAW(Small RAW2) + Middle Fine Jpeg" ,	[NSNumber numberWithInt:0x02640113] , 
			@"Small RAW(Small RAW2) + Small Fine Jpeg" ,	[NSNumber numberWithInt:0x02640213] , 
			@"Small RAW(Small RAW2) + Large Normal Jpeg" ,	[NSNumber numberWithInt:0x02640012] , 
			@"Small RAW(Small RAW2) + Middle Normal Jpeg" , [NSNumber numberWithInt:0x02640112] , 
			@"Small RAW(Small RAW2) + Small Normal Jpeg" ,	[NSNumber numberWithInt:0x02640212] , 

			@"RAW + Large Jpeg" ,		[NSNumber numberWithInt:0x00640010] , 
			@"RAW + Middle1 Jpeg" ,		[NSNumber numberWithInt:0x00640510] , 
			@"RAW + Middle2 Jpeg" ,		[NSNumber numberWithInt:0x00640610] , 
			@"RAW + Small Jpeg" ,		[NSNumber numberWithInt:0x00640210] , 
		
			@"Middle RAW + Large Jpeg" ,	[NSNumber numberWithInt:0x01640010] , 
			@"Middle RAW + Middle Jpeg" ,	[NSNumber numberWithInt:0x01640510] , 
			@"Middle RAW + Middle Jpeg" ,	[NSNumber numberWithInt:0x01640610] , 
			@"Middle RAW + Small Jpeg" ,	[NSNumber numberWithInt:0x01640210] , 
					 
			@"Small RAW + Large Jpeg" ,		[NSNumber numberWithInt:0x02640010] , 
			@"Small RAW + Middle1 Jpeg" ,	[NSNumber numberWithInt:0x02640510] , 
			@"Small RAW + Middle2 Jpeg" ,	[NSNumber numberWithInt:0x02640610] , 
			@"Small RAW + Small Jpeg" ,		[NSNumber numberWithInt:0x02640210] , 
					 
			@"Large Fine Jpeg" ,			[NSNumber numberWithInt:0x00130f0f] , 
			@"Middle Fine Jpeg" ,			[NSNumber numberWithInt:0x01130f0f] , 
			@"Small Fine Jpeg" ,			[NSNumber numberWithInt:0x02130f0f] , 
			@"Large Normal Jpeg" ,			[NSNumber numberWithInt:0x00120f0f] , 
			@"Middle Normal Jpeg" ,			[NSNumber numberWithInt:0x01120f0f] , 
			@"Small Normal Jpeg" ,			[NSNumber numberWithInt:0x02120f0f] , 

			@"Large Jpeg" ,				[NSNumber numberWithInt:0x00100f0f] , 
			@"Middle1 Jpeg" ,			[NSNumber numberWithInt:0x05100f0f] , 
			@"Middle2 Jpeg" ,			[NSNumber numberWithInt:0x06100f0f] , 
			@"Small Jpeg" ,				[NSNumber numberWithInt:0x02100f0f] , 

			// Legacy Camera
			@"RAW" , [NSNumber numberWithInt:0x00240000] , 
			@"RAW + Large Fine Jpeg" , [NSNumber numberWithInt:0x00240013] , 
			@"RAW + Middle Fine Jpeg" , [NSNumber numberWithInt:0x00240113] , 
			@"RAW + Small Fine Jpeg" , [NSNumber numberWithInt:0x00240213] , 
			@"RAW + Large Normal Jpeg" , [NSNumber numberWithInt:0x00240012] , 
			@"RAW + Middle Normal Jpeg" , [NSNumber numberWithInt:0x00240112] , 
			@"RAW + Small Normal Jpeg" , [NSNumber numberWithInt:0x00240212] , 
			@"Large Fine Jpeg" , [NSNumber numberWithInt:0x00130000] , 
			@"Middle Fine Jpeg" , [NSNumber numberWithInt:0x01130000] , 
			@"Small Fine Jpeg" , [NSNumber numberWithInt:0x02130000] , 
			@"Large Normal Jpeg" , [NSNumber numberWithInt:0x00120000] , 
			@"Middle Normal Jpeg" , [NSNumber numberWithInt:0x01120000] , 
			@"Small Normal Jpeg" , [NSNumber numberWithInt:0x02120000] , 

			@"RAW" , [NSNumber numberWithInt:0x002f000f] , 
			@"RAW + Large Jpeg" , [NSNumber numberWithInt:0x002f001f] , 

			@"RAW + Middle1 Jpeg" , [NSNumber numberWithInt:0x002f051f] , 
			@"RAW + Middle2 Jpeg" , [NSNumber numberWithInt:0x002f061f] , 
			@"RAW + Small Jpeg" , [NSNumber numberWithInt:0x002f021f] , 
			@"Large Jpeg" , [NSNumber numberWithInt:0x001f000f] , 
			@"Middle1 Jpeg" , [NSNumber numberWithInt:0x051f000f] , 
			@"Middle2 Jpeg" , [NSNumber numberWithInt:0x061f000f] , 
			@"Small Jpeg" , [NSNumber numberWithInt:0x021f000f] , nil];
			
}


@end
