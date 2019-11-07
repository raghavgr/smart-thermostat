//
//  ViewController.m
//  TSU
//
//  Created by r41p41 on 14/03/2019.
//  Copyright © 2019 r41p41. All rights reserved.
//

#import "ViewController.h"

@interface ViewController ()

@end

@implementation ViewController

#define L_Description 1030
#define L_Status 1031
#define SC_Status 1032
#define TB_TemperatureThreshold 1033

- (void) PeriodicUpdate
{
    NSURL *url = [NSURL URLWithString:@"http://rpande.co/TSU/state/temp.log"];
    NSString *htmlData = [NSString stringWithContentsOfURL:url];
    UILabel *label = [self.view viewWithTag:L_Description];
    if([htmlData characterAtIndex:0] == 'F' && [htmlData characterAtIndex:1] == ' ' && [htmlData characterAtIndex:2] == ':')
    {
        label.text = htmlData;
    }
    
    NSURLRequest *Request = [NSURLRequest requestWithURL:
                             [NSURL URLWithString: @"http://rpande.co/TSU/state/state.php?status=query"]];
    NSURLResponse *resp = nil;
    NSError *error = nil;
    label = [self.view viewWithTag:L_Status];
    NSData *response = [NSURLConnection sendSynchronousRequest: Request returningResponse: &resp error: &error];
    htmlData = [[NSString alloc] initWithData:response encoding:NSUTF8StringEncoding];
    
    UISegmentedControl *SC_Switch = [self.view viewWithTag:SC_Status];
    
    if([htmlData isEqualToString:@"on"])
    {
        label.text = @"On";
        if(SC_Switch.enabled == 0)
            SC_Switch.enabled = 1;
        SC_Switch.selectedSegmentIndex = 0;
    }
    if([htmlData isEqualToString:@"off"])
    {
        label.text = @"Off";
        if(SC_Switch.enabled == 0)
            SC_Switch.enabled = 1;
        SC_Switch.selectedSegmentIndex = 1;
    }
}


- (void)viewDidLoad
{
    [super viewDidLoad];
    //[self PeriodicUpdate];
    [NSTimer scheduledTimerWithTimeInterval:.5 target:self selector:@selector(PeriodicUpdate) userInfo:nil repeats:YES];
}
- (IBAction)Confirm:(id)sender
{
    NSURLRequest *Request = nil;
    NSURLResponse *resp = nil;
    NSError *error = nil;
    UITextField *TF_Temp = [self.view viewWithTag:TB_TemperatureThreshold];
    
    NSString *S_URL = [NSString stringWithFormat:@"http://rpande.co/TSU/state/state.php?threshold=%@",TF_Temp.text];
    Request = [NSURLRequest requestWithURL:
                   [NSURL URLWithString: S_URL]];
    if(Request != nil)
    {
        NSData *response = [NSURLConnection sendSynchronousRequest: Request returningResponse: &resp error: &error];
    }
}

- (IBAction)Callback_SwitchOnOff:(UISegmentedControl *)sender
{
    NSURLRequest *Request = nil;
    NSURLResponse *resp = nil;
    NSError *error = nil;
    if(sender.selectedSegmentIndex == 0)
    {
        Request = [NSURLRequest requestWithURL:
                   [NSURL URLWithString: @"http://rpande.co/TSU/state/state.php?state=on"]];
    }
    if(sender.selectedSegmentIndex == 1)
    {
        Request = [NSURLRequest requestWithURL:
                   [NSURL URLWithString: @"http://rpande.co/TSU/state/state.php?state=off"]];
    }
    if(Request != nil)
    {
        NSData *response = [NSURLConnection sendSynchronousRequest: Request returningResponse: &resp error: &error];
    }
}

@end
